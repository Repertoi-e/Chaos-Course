#include "window.h"

// clang-format off
#include <glad/glad.h>
#include <glfw/glfw3.h>
// clang-format on

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void *create_window(const char *title, int width, int height) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return null;
  defer_to_exit(glfwTerminate());

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow *window = glfwCreateWindow(width, height, title, null, null);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  gladLoadGL();
  return (void *)window;
}
