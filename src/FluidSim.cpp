#include "FluidSim.h"
#include "SDLWindow.h"
#include <glm/glm.hpp>

FluidSim::FluidSim()
{}

void FluidSim::init(int w, int h)
{
    width = w;
    height = h;

    velocity.create(w,h,GL_RG16F);
    dye.create(w,h,GL_RG16F);
    pressure.create(w,h,GL_RG16F);
    divergence.create(w,h,GL_RG16F);

    advectShader = new Shader("/home/s5729748/Desktop/Bournemouth/2Term/Simulation/Fluid-Simulation/shaders/vertex.glsl","/home/s5729748/Desktop/Bournemouth/2Term/Simulation/Fluid-Simulation/shaders/advectionFragment.glsl");
    displayShader = new Shader("/home/s5729748/Desktop/Bournemouth/2Term/Simulation/Fluid-Simulation/shaders/vertex.glsl","/home/s5729748/Desktop/Bournemouth/2Term/Simulation/Fluid-Simulation/shaders/fragment.glsl");
}


void FluidSim::update(float dt)
{
    advect(dye, velocity.read().getTexture(), dt);
    dye.swap();
}

void FluidSim::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,width,height);

    displayShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dye.read().getTexture());
    displayShader->setInt("uTexture", 0);

    RenderQuad();
}

void FluidSim::advect(PingPongBuffer& target, GLuint velocityTex, float dt)
{
    target.write().bind();

    advectShader->use();
    advectShader->setFloat("dt", dt);
    advectShader->setVec2("texelSize", glm::vec2(1.0f / width, 1.0f / height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocityTex);
    advectShader->setInt("uVelocity", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, target.read().getTexture());
    advectShader->setInt("uSource", 1);

    RenderQuad();
}