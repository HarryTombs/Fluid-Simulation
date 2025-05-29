#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <vector>
#include "SDLWindow.h"
#include "Shaders.h"
#include "ShaderUtils.h"

int ScreenHeight = 512;
int ScreenWidth = 512;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

GLuint texA, texB;
GLuint computeShader, renderShader, quadVAO;

bool ping;

float glX =  - 1.0f;
float glY = 1.0f;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

float verticies[] = {
    1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
};

unsigned int VBO;
unsigned int VAO;


void CheckSDLError(const std::string& message) {
    const char* error = SDL_GetError();
    if (*error != '\0') {
        std::cerr << "SDL Error (" << message << "): " << error << std::endl;
        SDL_ClearError();
        exit(1);
    }
}

void CheckGLError(const std::string& message) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error (" << message << "): " << error << std::endl;
        exit(1);
    }
}

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void createTexture(GLuint& tex) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, ScreenWidth, ScreenHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::vector<float> data(ScreenWidth * ScreenHeight * 4, 0.0f);
    data[(ScreenWidth / 2 + ScreenHeight / 2 * ScreenWidth) * 4 + 2] = 1.0f; // Initial "W" value in blue
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ScreenWidth, ScreenHeight, GL_RGBA, GL_FLOAT, data.data());
}

void InitialiseProgram() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL Video Subsystem: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_Init");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    GraphicsApplicationWindow = SDL_CreateWindow("GLWindow", 0, 0, ScreenHeight, ScreenWidth, SDL_WINDOW_OPENGL);
    if (GraphicsApplicationWindow == nullptr) {
        std::cerr << "SDL_Window was not created: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_CreateWindow");

    OpenGlConext = SDL_GL_CreateContext(GraphicsApplicationWindow);
    if (OpenGlConext == nullptr) {
        std::cerr << "OpenGL Context was not created: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_GL_CreateContext");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    CheckGLError("glewInit");

    GetOpenGLVersionInfo();
    std::cout << "OpenGL initialized successfully!" << std::endl;

    createTexture(texA);
    createTexture(texB);

    glGenBuffers(1, &VBO);
    CheckGLError("glGenBuffers");
    glGenVertexArrays(1, &VAO);
    CheckGLError("glGenVertexArrays");
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    CheckGLError("Buffer setup");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    CheckGLError("VertexAttribPointer (position)");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    CheckGLError("VertexAttribPointer (texture)");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    computeShader = loadComputeShader("../shaders/compute.glsl");
    renderShader = loadShaderProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    ping = true;

}

void Input() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "Bye!" << std::endl;
            gQuit = true;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {

            if (e.button.button == SDL_BUTTON_LEFT) {
                float glX = (e.motion.x / (float)ScreenHeight);
                float glY = (e.motion.y/ ((float)ScreenWidth)*-1.0f + 1.0f);  
                // renderToFramebuffer(writeFbo, OtherDraw, glX, glY);
            }
        }
        if (e.type == SDL_MOUSEMOTION) {
            if (e.motion.state & SDL_BUTTON_LMASK) {
                float glX = (e.motion.x / (float)ScreenHeight);
                float glY = (e.motion.y/ ((float)ScreenWidth)*-1.0f + 1.0f);
                // renderToFramebuffer(writeFbo, OtherDraw, glX, glY);
                // CheckGLError("Mouse motion rendering");
                // std::cout << "Mouse Position: (" << glX << ", " << glY << ")" << std::endl;
            }
        }
    }
}

void RenderQuad() {


}

void MainLoop() {
    while (!gQuit) {
        Input();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000) / SDL_GetPerformanceFrequency();
        deltaTime /= 1000.0;

        // std::cout << "Delta Time: " << deltaTime << " seconds" << std::endl;


        glUseProgram(computeShader);
        glBindImageTexture(0, ping ? texA : texB, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, ping ? texB : texA, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glDispatchCompute(ScreenWidth, ScreenHeight, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        CheckGLError("Compute Shader Dispatch");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(renderShader);
        glBindTexture(GL_TEXTURE_2D, ping ? texB : texA);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        CheckGLError("RenderQuad");
        SDL_GL_SwapWindow(GraphicsApplicationWindow);
        
        // std::cout << "Ping: " << ping << std::endl;

        ping = !ping;
        

        // RenderQuad();


    }
}

void CleanUp() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    SDL_GL_DeleteContext(OpenGlConext);
    SDL_DestroyWindow(GraphicsApplicationWindow);
    SDL_Quit();
}

int main() {
    InitialiseProgram();
    MainLoop();
    CleanUp();
    return 0;
}