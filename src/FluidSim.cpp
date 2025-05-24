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

    advectShader = std::make_unique<Shader>("../shaders/vertex.glsl", "../shaders/advectionFragment.glsl");
    displayShader = std::make_unique<Shader>("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    velocity.write().bind();
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f); // Clear to black
    glClear(GL_COLOR_BUFFER_BIT);
    velocity.swap();

    dye.write().bind();
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Initialize with red dye
    glClear(GL_COLOR_BUFFER_BIT);
    dye.swap();
}


void FluidSim::update(float dt)
{
    std::cout << "Updating simulation with dt: " << dt << std::endl;

    advect(dye, velocity.read().getTexture(), dt);
    dye.swap(); // Swap the buffers to use the updated texture
}

void FluidSim::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    displayShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dye.read().getTexture());
    displayShader->setInt("uTexture", 0);

    RenderQuad();
}

void FluidSim::advect(PingPongBuffer& target, GLuint velocityTex, float dt)
{
    target.write().bind();
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete! Status: " << status << std::endl;
    }

    advectShader->use();
    advectShader->setFloat("dt", dt);
    advectShader->setVec2("texelSize", glm::vec2(1.0f / width, 1.0f / height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocityTex);
    advectShader->setInt("uVelocity", 0);

    GLint width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    // std::cout << "Velocity Texture Size: " << width << "x" << height << std::endl;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, target.read().getTexture());
    advectShader->setInt("uSource", 1);

    RenderQuad();
}