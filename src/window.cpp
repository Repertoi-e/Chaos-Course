#include "window.h"

#include "defer.h"

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void *create_window(const char *title, int width, int height)
{
    glfwInit();
    defer_to_exit(glfwTerminate());

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *window = glfwCreateWindow(width, height, "Mandelbrot Set", null, null);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    return (void *) window;
}
