#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "SDLWindow.h"
#include "Shaders.h"
#include "Framebuffer.h"




int ScreenHeight = 640;
int ScreenWidth = 480;
SDL_Window* GraphicsApplicationWindow = nullptr;
SDL_GLContext OpenGlConext = nullptr;
bool gQuit = false;



float verticies[] = 
{
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
       -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 
       -1.0f, -1.0f, 0.0f, 
       -1.0f,  1.0f, 0.0f   
};


unsigned int VBO;
unsigned int VAO;

Framebuffer dyeFBO;

Shader* finalDraw = nullptr;
// Shader* advectShader = nullptr;
// Shader* addForce = nullptr;
// Shader* divergence = nullptr;
// Shader* jacobiIteration = nullptr;
// Shader* subtract = nullptr;


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
    std::cout << "OpenGL initialized successfully!" << std::endl;



    finalDraw = new Shader("/home/s5729748/Desktop/Bournemouth/Term2/Simulation/Fluid-Simulation/shaders/vertex.glsl","/home/s5729748/Desktop/Bournemouth/Term2/Simulation/Fluid-Simulation/shaders/fragment.glsl");

    // advectShader = new Shader("/home/harry/Documents/CPP/Fluid-Simulation/shaders/vertex.glsl","/home/harry/Documents/CPP/Fluid-Simulation/shaders/advectionFragment.glsl");
    // addForce = new Shader("/home/harry/Documents/CPP/Fluid-Simulation/shaders/vertex.glsl","/home/harry/Documents/CPP/Fluid-Simulation/shaders/addForceFragment.glsl");
    // divergence = new Shader("/home/harry/Documents/CPP/Fluid-Simulation/shaders/vertex.glsl","/home/harry/Documents/CPP/Fluid-Simulation/shaders/diverganceFragment.glsl");
    // jacobiIteration = new Shader("/home/harry/Documents/CPP/Fluid-Simulation/shaders/vertex.glsl","/home/harry/Documents/CPP/Fluid-Simulation/shaders/jacobiIterationFragment.glsl");
    // subtract = new Shader("/home/harry/Documents/CPP/Fluid-Simulation/shaders/vertex.glsl","/home/harry/Documents/CPP/Fluid-Simulation/shaders/subtractFragment.glsl");
    glGenBuffers(1,&VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    dyeFBO.create(ScreenWidth,ScreenHeight,GL_RGBA16F);

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

        // per frame 
        // 1 advection of dye and velocity, out put new velocity 
        // calculate external force (mouse input) apply to velocity, output new
        // measure the divergance, (difference of inflow and outflow) output divergance
        // PER ITERATION, "smooth" the divergance till its a normal amount 20-40 times per cell
        // correct velocity based on smoothed divergance
        // render the dye to screen
        // this is all in glsl

        // you need FBOs to save them all to like in deffered shading
        // Make an FBO class and a fluid class to do that

        finalDraw->use();
        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
