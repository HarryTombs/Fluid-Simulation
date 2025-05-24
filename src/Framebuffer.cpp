#include "Framebuffer.h"
#include <iostream>

void Framebuffer::create(int w, int h, GLenum format, bool useDepth)
{
    width = w;
    height = h;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum dataFormat;
    if (format == GL_R16F || format == GL_RED)
        dataFormat = GL_RED;
    else if (format == GL_RG16F || format == GL_RG)
        dataFormat = GL_RG;
    else
        dataFormat = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, dataFormat, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    if (useDepth) {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete! Status: " << status << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0,0,width,height);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::destroy()
{
    if (texture) glDeleteTextures(1, &texture);
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (rbo) glDeleteRenderbuffers(1, &rbo);
    texture = fbo = rbo = 0;
}

void Framebuffer::resize(int newWidth, int newHeight, GLenum format, bool useDepth)
{
    destroy();
    create(newWidth,newHeight,format,useDepth);
}
