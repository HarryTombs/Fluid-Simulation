#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <GL/glew.h>
#include <GL/glut.h>

class Framebuffer 
{
public:
    GLuint fbo = 0;
    GLuint texture = 0;
    GLuint rbo = 0; 
    int width = 0, height = 0;

    void create(int w, int h, GLenum format = GL_RGBA16F, bool useDepth = false);
    void bind();
    void unbind();
    void destroy();

    void resize(int newWidth, int newHeight, GLenum format = GL_RGBA16F, bool useDepth = false);

    GLuint getTexture() const { return texture; }

};

struct PingPongBuffer {
    Framebuffer buffers[2];
    int writeIndex = 0;

    void create(int w, int h, GLenum format) {
        buffers[0].create(w, h, format,false);
        buffers[1].create(w, h, format,false);
    }

    void swap() { writeIndex = 1 - writeIndex; }

    Framebuffer& read() { return buffers[1 - writeIndex]; }
    Framebuffer& write() { return buffers[writeIndex]; }
};


#endif