#define STB_DS_IMPLEMENTATION

#include "window.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>

int WIDTH = 1920;
int HEIGHT = 1080;

GLuint g_FrameBuffer, g_ColorTexture;
GLuint g_Program, g_VertexArray, g_VertexBuffer;

bool init_resources()
{
    shader_segment *segments = read_shader_file("data/week2.shader");
    g_Program = create_shader(segments);
    if (!g_Program)
        return false;
    defer_to_exit(glDeleteProgram(g_Program));

    // Set up vertex buffer
    glGenVertexArrays(1, &g_VertexArray);
    glBindVertexArray(g_VertexArray);
    defer_to_exit(glDeleteVertexArrays(1, &g_VertexArray));

    glGenBuffers(1, &g_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);
    defer_to_exit(glDeleteBuffers(1, &g_VertexBuffer));

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
    glUseProgram(g_Program);

    GLint screenDimLocation = glGetUniformLocation(g_Program, "screen_dim");
    glUniform2f(screenDimLocation, (float)WIDTH, (float)HEIGHT);

    // Set up framebuffer
    glGenFramebuffers(1, &g_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer);
    defer_to_exit(glDeleteFramebuffers(1, &g_FrameBuffer));

    glGenTextures(1, &g_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
    defer_to_exit(glDeleteTextures(1, &g_ColorTexture));

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, null);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_ColorTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error: Failed to create framebuffer.\n");
        glfwTerminate();
        return false;
    }

    return true;
}

int main()
{
    auto *window = (GLFWwindow *) create_window("Week 2", WIDTH, HEIGHT);
    if (!window)
        return 1;
        
    if (!init_resources())
        return 1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_FrameBuffer);
        glViewport(0, 0, WIDTH, HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    

        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_FrameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window);
    }


    // Draw a frame and save to .ppm file
    GLubyte *pixels = new GLubyte[3 * WIDTH * HEIGHT];
    defer(delete[] pixels);

    glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    void save_frame_to_ppm(const char *filename, GLubyte *pixels, int width, int height);
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