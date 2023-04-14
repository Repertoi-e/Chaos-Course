#include <glad/glad.h>
#include <imgui.h>
#include <lstd/lstd.h>

#include "shader.h"
#include "state.h"
#include "window.h"

extern int WIDTH;
extern int HEIGHT;

int get_width() {
  if (g_State.ViewportSize.x == 0.0f) {
    return WIDTH;
  }
  return (int)g_State.ViewportSize.x;
}

int get_height() {
  if (g_State.ViewportSize.y == 0.0f) {
    return HEIGHT;
  }
  return (int)g_State.ViewportSize.y;
}

GLuint g_FrameBuffer = 0, g_ColorTexture = 0;
GLuint g_Program, g_VertexArray, g_VertexBuffer;

void create_vb() {
  glGenVertexArrays(1, ref g_VertexArray);
  glBindVertexArray(g_VertexArray);
  defer_to_exit(glDeleteVertexArrays(1, ref g_VertexArray));

  glGenBuffers(1, ref g_VertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);
  defer_to_exit(glDeleteBuffers(1, ref g_VertexBuffer));

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

bool destroy_if_it_exists_and_create_framebuffer() {
  if (g_FrameBuffer) glDeleteFramebuffers(1, ref g_FrameBuffer);
  if (g_ColorTexture) glDeleteTextures(1, ref g_ColorTexture);

  glGenFramebuffers(1, ref g_FrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, g_FrameBuffer);
  defer_to_exit(glDeleteFramebuffers(1, ref g_FrameBuffer));

  glGenTextures(1, ref g_ColorTexture);
  glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
  defer_to_exit(glDeleteTextures(1, ref g_ColorTexture));

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, get_width(), get_height(), 0, GL_RGB,
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

void mandlebrot_layer_viewport_resized() {
  destroy_if_it_exists_and_create_framebuffer();
  upload_screen_dim_to_shader();
}

bool mandlebrot_layer_init() {
  array<shader_segment> segments = read_shader_file("data/mandlebrot.shader");
  defer(free(segments));

  g_Program = create_shader(segments);
  if (!g_Program) return false;
  defer_to_exit(glDeleteProgram(g_Program));

  create_vb();
  upload_screen_dim_to_shader();

  return destroy_if_it_exists_and_create_framebuffer();
}

void mandlebrot_layer_update() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_FrameBuffer);
  glViewport(0, 0, get_width(), get_height());
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_FrameBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  if (!g_State.DrawEditorUI) {
    // Draw directly to window if we aren't drawing the editor
    glBlitFramebuffer(0, 0, get_width(), get_height(), 0, 0, get_width(),
                      get_height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
  }
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
  GLubyte *pixels = new GLubyte[3 * get_width() * get_height()];
  defer(delete[] pixels);

  glBindTexture(GL_TEXTURE_2D, g_ColorTexture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  save_frame_to_ppm("mandelbrot.ppm", pixels, get_width(), get_height());
}

void mandlebrot_layer_ui() {
  ImGui::Begin("Hello");
  ImGui::End();

  // Draw to viewport window if we aren drawing the editor
  if (g_State.DrawEditorUI) {
    ImGui::Begin("Viewport");

    ImVec2 viewportPos = ImGui::GetWindowPos();
    ImVec2 viewportSize = ImGui::GetWindowSize();

    auto *d = ImGui::GetWindowDrawList();
    d->AddImage(
        (ImTextureID)(intptr_t)g_FrameBuffer, viewportPos,
        {viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y});

    ImGui::End();
  }
}
