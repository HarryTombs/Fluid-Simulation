// filepath: /home/harry/Documents/CPP/SDL_Proj/include/SDLWindow.h
#ifndef SDLWINDOW_H
#define SDLWINDOW_H
#include <SDL2/SDL.h>
#include "Shaders.h"

void GetOpenGLVersionInfo();
void InitialiseProgram();
void Input();
void RenderQuad();
void MainLoop();
void CleanUp();

void renderToFramebuffer(int Framebuffer, Shader* shader, float glX, float glY);



#endif // SDLWINDOW_H