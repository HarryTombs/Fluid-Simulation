#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "SDLWindow.h"
#include "Shaders.h"

int ScreenHeight = 512;
int ScreenWidth = 512;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

GLuint texA, texB;
GLuint computeShader, renderShader, quadVAO;


unsigned int fbo1;
unsigned int fbo2;

unsigned int fbo1Texture;
unsigned int fbo2Texture;

bool ping;

unsigned int readTex;
unsigned int readFbo;
unsigned int writeFbo;

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
Shader* FrameBufferDraw = nullptr;

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

    glewExperimental = GL_TRUE;
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

    OtherDraw = new Shader("../shaders/framebufferVertex.glsl", "../shaders/advectionFragment.glsl");
    if (!OtherDraw) {
        std::cerr << "Failed to create OtherDraw shader!" << std::endl;
        exit(1);
    }
    CheckGLError("Shader creation (OtherDraw)");

    FrameBufferDraw = new Shader("../shaders/framebufferVertex.glsl", "../shaders/framebufferFragment.glsl");
    if (!FrameBufferDraw) {
        std::cerr << "Failed to create FrameBufferDraw shader!" << std::endl;
        exit(1);
    }
    CheckGLError("Shader creation (FrameBufferDraw)");

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

    glGenFramebuffers(1, &fbo1);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);

    glGenTextures(1, &fbo1Texture);
    glBindTexture(GL_TEXTURE_2D, fbo1Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ScreenHeight, ScreenWidth, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo1Texture, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, ScreenHeight, ScreenWidth);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer 1 not complete! Status: " << status << std::endl;
        exit(1);
    }
    CheckGLError("Framebuffer 1 setup");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGLError("glGenFramebuffers");

    // Create second framebuffer

    glGenFramebuffers(1, &fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);

    glGenTextures(1, &fbo2Texture);
    glBindTexture(GL_TEXTURE_2D, fbo2Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, ScreenHeight, ScreenWidth, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo2Texture, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, ScreenHeight, ScreenWidth);
    GLenum status2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status2 != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer 2 not complete! Status: " << status2 << std::endl;
        exit(1);
    }
    CheckGLError("Framebuffer 2 setup");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CheckGLError("glGenFramebuffers");

    readTex = fbo1Texture;
    writeFbo = fbo2;
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
                renderToFramebuffer(writeFbo, OtherDraw, glX, glY);
            }
        }
        if (e.type == SDL_MOUSEMOTION) {
            if (e.motion.state & SDL_BUTTON_LMASK) {
                float glX = (e.motion.x / (float)ScreenHeight);
                float glY = (e.motion.y/ ((float)ScreenWidth)*-1.0f + 1.0f);
                renderToFramebuffer(writeFbo, OtherDraw, glX, glY);
                CheckGLError("Mouse motion rendering");
                // std::cout << "Mouse Position: (" << glX << ", " << glY << ")" << std::endl;
            }
        }
    }
}

void renderToFramebuffer(int Framebuffer, Shader* shader, float glX, float glY) {
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    CheckGLError("glBindFramebuffer");
    

    glViewport(0, 0, ScreenHeight, ScreenWidth);
    CheckGLError("glViewport");

    shader->use();
    CheckGLError("Shader use");

    shader->setVec2("mousePos", glm::vec2(glX, glY));
    CheckGLError("Set mouse position");
    shader->setInt("previousFrame", readTex);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderQuad() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    CheckGLError("RenderQuad");
    SDL_GL_SwapWindow(GraphicsApplicationWindow);

}

void MainLoop() {
    while (!gQuit) {
        Input();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000) / SDL_GetPerformanceFrequency();
        deltaTime /= 1000.0;

        // std::cout << "Delta Time: " << deltaTime << " seconds" << std::endl;

        

        if (ping)
        {
            readTex = fbo1Texture;
            readFbo = fbo1;
            writeFbo = fbo2;
            std::cout << "Using FBO 1" << std::endl;
        }
        else 
        {
            readTex = fbo2Texture;
            readFbo = fbo2;
            writeFbo = fbo1;
            std::cout << "Using FBO 2" << std::endl;
        }


        ping = !ping;

        // std::cout << "Ping: " << ping << std::endl;
        // std::cout << "FBO: " << writeFbo << std::endl;
        // std::cout << "Tex: " << readTex << std::endl;

        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ScreenHeight, ScreenWidth);
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, readFbo);
        glClearColor(float(ping), 0.0f, 0.0f, 1.0f);
        std::cout <<"Ping: " << ping << std::endl;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, readTex);

        OtherDraw->use();
        OtherDraw->setInt("previousFrame", fbo2Texture);
        RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0 );


        finalDraw->use();
        finalDraw->setInt("previousFrame", readTex);
        RenderQuad();


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