#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <vector>
#include "SDLWindow.h"
#include "ShaderUtils.h"

int ScreenHeight = 512;
int ScreenWidth = 512;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

GLuint divergence;
GLuint curl;
GLuint velocityA, velocityB;
GLuint densityA, densityB;
GLuint pressureA, pressureB;

GLuint computeShader, divergenceShader, jacobiShader, GradientShader;

GLuint renderShader, quadVAO;

bool ping;
bool jacobiping;


float glX = - 1.0f;
float glY = - 1.0f;
float glXLast = -1.0f;
float glYLast = -1.0f;
float glXDelta = 0.0f;
float glYDelta = 0.0f;
bool mouseDown = false;

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

float verticies[] = 
{
    1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
   -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 1.0f
};

unsigned int VBO;
unsigned int VAO;


void CheckSDLError(const std::string& message) 
{
    const char* error = SDL_GetError();
    if (*error != '\0') 
    {
        std::cerr << "SDL Error (" << message << "): " << error << std::endl;
        SDL_ClearError();
        exit(1);
    }
}

void CheckGLError(const std::string& message) 
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) 
    {
        std::cerr << "OpenGL Error (" << message << "): " << error << std::endl;
        exit(1);
    }
}

void GetOpenGLVersionInfo() 
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void createTexture(GLuint& tex) 
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, ScreenWidth, ScreenHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError("createTexture");
}

void createTexture1F(GLuint& tex)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, ScreenWidth, ScreenHeight);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError("createTexture1F");
}
void InitialiseProgram() 
{
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

    GraphicsApplicationWindow = SDL_CreateWindow("GLWindow", 0, 0, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL);
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


    createTexture(velocityA);
    createTexture(velocityB);
    createTexture1F(pressureA);
    createTexture1F(pressureB);
    createTexture1F(divergence);


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

    renderShader = loadShaderProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    computeShader = loadComputeShader("../shaders/compute.glsl");
    divergenceShader = loadComputeShader("../shaders/divergence.glsl");
    jacobiShader = loadComputeShader("../shaders/jacobi.glsl");

    ping = true;
    

}

void Input() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) 
    {
        if (e.type == SDL_QUIT) 
        {
            std::cout << "Bye!" << std::endl;
            gQuit = true;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) 
        {

            if (e.button.button == SDL_BUTTON_LEFT) 
            {
                mouseDown = true;
                glX = ( e.motion.x); 
                glY = ((ScreenHeight - e.motion.y));
            }
        }
        if (e.type == SDL_MOUSEMOTION) 
        {
            if (e.motion.state & SDL_BUTTON_LMASK) 
            {
                glX = (e.motion.x);  
                glY = ((ScreenHeight - e.motion.y)); 
            }
            
        }
        if (e.type == SDL_MOUSEBUTTONUP) 
        {
            if (e.button.button == SDL_BUTTON_LEFT) 
            {
                mouseDown = false;
                glX = -1.0f; 
                glY = -1.0f;  
                
            }
        }
    }
}

void setMouseUniform(GLuint shader)
{
    GLuint mouseLoc = glGetUniformLocation(shader, "mousePos");
    if (mouseLoc != -1) 
    {
        glUniform2f(mouseLoc, glX, glY);
    }
    GLuint mousePress = glGetUniformLocation(shader, "mousePress");
    if (mousePress != -1) 
    {
        glUniform1i(mousePress,mouseDown ? 1 : 0);
    }
    GLuint mouseDeltaLoc = glGetUniformLocation(shader, "mouseDelta");
    glUniform2f(mouseDeltaLoc, glXDelta, glYDelta);
}



void MainLoop() {
    while (!gQuit) 
    {
        Input();

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((NOW - LAST) * 1000) / SDL_GetPerformanceFrequency();
        deltaTime /= 1000.0;

        glXDelta = glX - glXLast;
        glYDelta = glY - glYLast;

        glXLast = glX;
        glYLast = glY;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glUseProgram(computeShader);
        glBindImageTexture(0, ping ? velocityA : velocityB, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, ping ? velocityB : velocityA, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        GLuint mouseLoc = glGetUniformLocation(computeShader, "mousePos");
        if (mouseLoc != -1) {
            glUniform2i(mouseLoc, glX, glY);
        }
        CheckGLError("Uniform Mouse Position");
        GLuint mousePress = glGetUniformLocation(computeShader, "mousePress");
        if (mousePress != -1) {
            glUniform1i(mousePress,mouseDown ? 1 : 0);
        }
        glUniform2i(glGetUniformLocation(computeShader, "Resolution"), ScreenWidth, ScreenHeight);

        glDispatchCompute(ScreenWidth, ScreenHeight, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        CheckGLError("Compute Shader Dispatch");


        glUseProgram(divergenceShader);
        glBindImageTexture(0, ping ? velocityB : velocityA, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, divergence, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        glUniform2i(glGetUniformLocation(divergenceShader, "Resolution"), ScreenWidth, ScreenHeight);

        glDispatchCompute(ScreenWidth, ScreenHeight, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        CheckGLError("Divergence Shader Dispatch");

        jacobiping = true;

        for (int i = 0; i < 20; i++) 
        {
            glUseProgram(jacobiShader);
            glBindImageTexture(0, jacobiping ? pressureA : pressureB, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
            glBindImageTexture(1, jacobiping ? pressureA : pressureB, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
            glBindImageTexture(2, divergence, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);

            glUniform2i(glGetUniformLocation(jacobiShader, "Resolution"), ScreenWidth, ScreenHeight);

            glDispatchCompute(ScreenWidth, ScreenHeight, 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            CheckGLError("Jacobi Shader Dispatch");
            jacobiping = !jacobiping;
        }
        

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(renderShader);
        glBindTexture(GL_TEXTURE_2D, ping ? velocityA : velocityB);
        glUniform1i(glGetUniformLocation(renderShader, "tex"), 0);
        glUniform2i(glGetUniformLocation(renderShader, "Resolution"), ScreenWidth, ScreenHeight);
        CheckGLError("Bind Texture");
        glClearColor(0.8, 0.4, 0.15, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        CheckGLError("RenderQuad");
        SDL_GL_SwapWindow(GraphicsApplicationWindow);
        

        ping = !ping;



    }
}

void CleanUp() 
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(renderShader);
    SDL_GL_DeleteContext(OpenGlConext);
    SDL_DestroyWindow(GraphicsApplicationWindow);
    SDL_Quit();
}

int main() 
{
    InitialiseProgram();
    MainLoop();
    CleanUp();
    return 0;
}