#include <glad/glad.h>
#include <imgui.h>
#include <lstd/lstd.h>

#include "shader.h"
#include "state.h"
#include "window.h"

asset_key g_ShaderKey = string("data/mandlebrot.shader");
GLuint g_VertexArray, g_VertexBuffer;

bool mandlebrot_layer_init() {
  glGenVertexArrays(1, ref g_VertexArray);
  glBindVertexArray(g_VertexArray);

  glGenBuffers(1, ref g_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);

  GLfloat vertices[] = {
      -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, null);

  return true;
}

void mandlebrot_layer_uninit() {
  glDeleteBuffers(1, ref g_VertexBuffer);
  glDeleteVertexArrays(1, ref g_VertexArray);
}

void mandlebrot_layer_render_to_viewport() {
  GLuint program = get_shader_from_key(g_ShaderKey).Program;

  glUseProgram(program);
  glBindVertexArray(g_VertexArray);

  GLint screenDimLocation = glGetUniformLocation(program, "screen_dim");
  glUniform2f(screenDimLocation, (float)get_width(), (float)get_height());

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void mandlebrot_layer_ui() {}
