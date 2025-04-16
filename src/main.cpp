#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "SDLWindow.h"


int ScreenHeight = 640;
int ScreenWidth = 480;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;

float verticies[] = 
{
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f, 
       -0.5f, -0.5f, 0.0f, 
       -0.5f,  0.5f, 0.0f   
};

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

unsigned int shaderProgram;

unsigned int VBO;
unsigned int VAO;




void GetOpenGLVersionInfo(){
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}


void InitialiseProgram()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Could not initialise Video Subsystem" << std::endl; 
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);    

    GraphicsApplicationWindow = SDL_CreateWindow("GLWindow",0,0,ScreenHeight,ScreenWidth,SDL_WINDOW_OPENGL);

    if(GraphicsApplicationWindow == nullptr)
    {
        std::cout << "SDL_Window Was not created" << std::endl;
        exit(1);
    }

    OpenGlConext = SDL_GL_CreateContext(GraphicsApplicationWindow);

    if(OpenGlConext == nullptr)
    {
        std::cout << "OpenGL Context Was not created" << std::endl;
        exit(1);
    }

    glewExperimental=true;
    GLenum err = glewInit();
    if (err!=GLEW_OK) 
    {
        std::cout << "Glew init Failed: " << glewGetErrorString(err) << std::endl;
        exit(1);
    }
    GetOpenGLVersionInfo();


    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenBuffers(1,&VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

}

void Input()
{
    SDL_Event e; 
    while(SDL_PollEvent(&e) !=0)
    {
        if(e.type == SDL_QUIT)
        {
            std::cout << "Bye!" << std::endl;
            gQuit = true;
        }
    }
}

void MainLoop()
{
    while(!gQuit){
        Input();

        glClearColor(0.9f,0.7f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(GraphicsApplicationWindow);
    }
}


void CleanUp()
{
    SDL_DestroyWindow(GraphicsApplicationWindow);

    SDL_Quit();
}


int main(){
    InitialiseProgram();

    MainLoop();

    CleanUp();
    return 0;
}
