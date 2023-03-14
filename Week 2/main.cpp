#define STB_DS_IMPLEMENTATION

#include "window.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

int WIDTH = 1920 * 2;
int HEIGHT = 1080 * 2;

void save_frame_to_ppm(const char *filename, GLubyte *pixels, int width, int height);

int main()
{
    void *window = create_window("Week 2", WIDTH, HEIGHT);
    if (!window)
        return 1;

    defer(glfwTerminate());

    shader_segment *segments = read_shader_file("data/default.shader");
    uint program = create_shader(segments);
    if (!program)
        return 1;

    defer(glDeleteProgram(program));

    // Set up vertex buffer
    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    defer(glDeleteVertexArrays(1, &vertex_array));

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    defer(glDeleteBuffers(1, &vertex_buffer));

    GLfloat vertices[] = {
        -1.0,
        -1.0,
        1.0,
        -1.0,
        -1.0,
        1.0,
        1.0,
        1.0,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, null);

    // Set up uniform variables
    glUseProgram(program);

    GLint screenDimLocation = glGetUniformLocation(program, "screen_dim");
    glUniform2f(screenDimLocation, (float) WIDTH, (float) HEIGHT);

    // Set up framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    defer(glDeleteFramebuffers(1, &framebuffer));

    GLuint color_texture;
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    defer(glDeleteTextures(1, &color_texture));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, null);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error: Failed to create framebuffer.\n");
        glfwTerminate();
        return 1;
    }

    // Draw a frame and save to .ppm file
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    GLubyte *pixels = new GLubyte[3 * WIDTH * HEIGHT];
    defer(delete[] pixels);

    glBindTexture(GL_TEXTURE_2D, color_texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    save_frame_to_ppm("mandelbrot.ppm", pixels, WIDTH, HEIGHT);

    return 0;
}

void save_frame_to_ppm(const char *filename, GLubyte *pixels, int width, int height)
{
    FILE *fp;
    fp = fopen(filename, "wb");

    if (!fp)
    {
        fprintf(stderr, "Failed to open file for writing: %s\n", filename);
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    fwrite(pixels, 3 * width * height, sizeof(GLubyte), fp);

    fclose(fp);
}