#include <glad/glad.h>
#include <imgui.h>
#include <lstd/lstd.h>

#include "shader.h"
#include "state.h"
#include "window.h"

GLuint g_Program, g_VertexArray, g_VertexBuffer;

void create_vb() {
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
}

void upload_screen_dim_to_shader() {
  glUseProgram(g_Program);

  GLint screenDimLocation = glGetUniformLocation(g_Program, "screen_dim");
  glUniform2f(screenDimLocation, (float)get_width(), (float)get_height());
}

void mandlebrot_layer_viewport_resized() { upload_screen_dim_to_shader(); }

bool mandlebrot_layer_init() {
  array<shader_segment> segments = read_shader_file("data/mandlebrot.shader");
  defer(free(segments));

  g_Program = create_shader(segments);
  if (!g_Program) return false;

  create_vb();
  upload_screen_dim_to_shader();

  return true;
}

void mandlebrot_layer_uninit() {
  glDeleteProgram(g_Program);
  glDeleteBuffers(1, ref g_VertexBuffer);
  glDeleteVertexArrays(1, ref g_VertexArray);
}

void mandlebrot_layer_render_to_viewport() {
  glUseProgram(g_Program);
  glBindVertexArray(g_VertexArray);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void mandlebrot_layer_ui() {}
