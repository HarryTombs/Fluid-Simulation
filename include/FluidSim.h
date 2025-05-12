#ifndef FLUIDSIM_H
#define FLUIDSIM_H
#include "Framebuffer.h"
#include "Shaders.h"
#include <GL/glew.h>
#include <GL/glut.h>

class FluidSim
{
public:
    FluidSim();
    int width, height;

    PingPongBuffer velocity;
    PingPongBuffer dye;
    Framebuffer divergence;
    PingPongBuffer pressure;

    Shader* advectShader;
    Shader* displayShader;

    void init(int w, int h);
    void update(float dt);
    void render();
    void advect(PingPongBuffer& target, GLuint velocityTex, float dt);

private: 
};

#endif