#include <glad/glad.h>
#include <imgui.h>
#include <lstd/lstd.h>

#include "shader.h"
#include "window.h"

extern int WIDTH;
extern int HEIGHT;

GLuint g_FrameBuffer, g_ColorTexture;
GLuint g_Program, g_VertexArray, g_VertexBuffer;

void create_vb() {
  glGenVertexArrays(1, &g_VertexArray);
  glBindVertexArray(g_VertexArray);
  defer_to_exit(glDeleteVertexArrays(1, &g_VertexArray));

  glGenBuffers(1, &g_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);
  defer_to_exit(glDeleteBuffers(1, &g_VertexBuffer));

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
  glUniform2f(screenDimLocation, (float)WIDTH, (float)HEIGHT);
}

bool create_framebuffer() {
  glGenFramebuffers(1, &g_FrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer);
  defer_to_exit(glDeleteFramebuffers(1, &g_FrameBuffer));

  glGenTextures(1, &g_ColorTexture);
  glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
  defer_to_exit(glDeleteTextures(1, &g_ColorTexture));

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, null);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         g_ColorTexture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Error: Failed to create framebuffer.\n");
    return false;
  }
  return true;
}

bool mandlebrot_layer_init() {
  array<shader_segment> segments = read_shader_file("data/mandlebrot.shader");
  defer(free(segments));

  g_Program = create_shader(segments);
  if (!g_Program) return false;
  defer_to_exit(glDeleteProgram(g_Program));

  create_vb();
  upload_screen_dim_to_shader();

  return create_framebuffer();
}

void mandlebrot_layer_update() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_FrameBuffer);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_FrameBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void save_frame_to_ppm(string path, GLubyte *pixels, int width, int height) {
  string header = tprint("P6\n{} {}\n255\n", width, height);

  s64 size = header.Count + (width * height * 3);
  string contents;
  contents.Data = malloc<char>({.Count = size, .Alloc = TemporaryAllocator});
  contents.Count = size;
  memcpy(contents.Data, header.Data, header.Count);
  memcpy(contents.Data + header.Count, header.Data, header.Count);

  bool success =
      os_write_to_file(path, contents, file_write_mode::Overwrite_Entire);
}

void on_click() {
  // Draw a frame and save to .ppm file
  GLubyte *pixels = new GLubyte[3 * WIDTH * HEIGHT];
  defer(delete[] pixels);

  glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  save_frame_to_ppm("mandelbrot.ppm", pixels, WIDTH, HEIGHT);
}

void mandlebrot_layer_ui() {
  ImGui::Begin("Hello");
  ImGui::End();
}
