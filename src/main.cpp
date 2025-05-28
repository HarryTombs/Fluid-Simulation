#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "SDLWindow.h"
#include "Shaders.h"
#include "Framebuffer.h"
#include "FluidSim.h"

int ScreenHeight = 640;
int ScreenWidth = 480;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

Framebuffer FboA;
Framebuffer FboB;

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

Shader* finalDraw = nullptr;
Shader* OtherDraw = nullptr;

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

void InitialiseProgram() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL Video Subsystem: " << SDL_GetError() << std::endl;
        exit(1);
    }
    CheckSDLError("SDL_Init");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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

    glewExperimental = true;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    CheckGLError("glewInit");

    GetOpenGLVersionInfo();
    std::cout << "OpenGL initialized successfully!" << std::endl;

    finalDraw = new Shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    if (!finalDraw) {
        std::cerr << "Failed to create finalDraw shader!" << std::endl;
        exit(1);
    }
    CheckGLError("Shader creation (finalDraw)");

    OtherDraw = new Shader("../shaders/vertex.glsl", "../shaders/advectionFragment.glsl");
    if (!OtherDraw) {
        std::cerr << "Failed to create OtherDraw shader!" << std::endl;
        exit(1);
    }
    CheckGLError("Shader creation (OtherDraw)");

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

    FboA.create(ScreenHeight, ScreenWidth, GL_RGBA32F,true);
    FboB.create(ScreenHeight,ScreenWidth,GL_RGBA32F,true);

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
                glX = (e.motion.x / (float)ScreenHeight);
                glY = (e.motion.y/ ((float)ScreenWidth)*-1.0f + 1.0f);  
            }
        }
        if (e.type == SDL_MOUSEMOTION) {
            if (e.motion.state & SDL_BUTTON_LMASK) {
                glX = (e.motion.x / (float)ScreenHeight);
                glY = (e.motion.y/ ((float)ScreenWidth)*-1.0f + 1.0f);
            }
        }
    }
}

void RenderQuad() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    CheckGLError("RenderQuad");
}

void MainLoop() {
    while (!gQuit) {
        Input();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000) / SDL_GetPerformanceFrequency();
        deltaTime /= 1000.0;

        // SIM PASS

        GLuint readTex = ping ? FboA.getTexture() : FboB.getTexture();
        GLuint writeFBO = ping ? FboB.getFBO() : FboA.getFBO();

        glBindFramebuffer(GL_FRAMEBUFFER,writeFBO);
        glViewport(0,0,ScreenHeight,ScreenWidth);
        CheckGLError("Framebuffer bind");
        OtherDraw->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, readTex);
        OtherDraw->setVec2("mousePos", glm::vec2(glX, glY));
        OtherDraw->setVec2("iResolution", glm::vec2(ScreenHeight,ScreenWidth));
        OtherDraw->setInt("iChannel0",0);
        OtherDraw->setFloat("iTime", deltaTime);
        OtherDraw->use();
        RenderQuad();

        ping = !ping;

        // glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        CheckGLError("Framebuffer unbind");



        glViewport(0, 0, ScreenHeight, ScreenWidth);

        finalDraw->use();

        GLuint currentTex = ping ? FboB.getTexture() : FboA.getTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTex);

        finalDraw->setFloat("iChannel0", 0);
        finalDraw->setVec2("iResolution",glm::vec2(ScreenHeight,ScreenWidth));
        
        // glClearColor(0.0f, 0.7f, 0.7f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // CheckGLError("MainLoop (clear)");

        RenderQuad();
        SDL_GL_SwapWindow(GraphicsApplicationWindow);


    }
}

void CleanUp() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    SDL_DestroyWindow(GraphicsApplicationWindow);
    SDL_Quit();
}

int main() {
    InitialiseProgram();
    MainLoop();
    CleanUp();
    return 0;
}