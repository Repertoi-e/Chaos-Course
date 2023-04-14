// Emedded font
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
// clang-format off
#include <glad/glad.h>
#include <glfw/glfw3.h>
// clang-format on
#include <imgui.h>
#include <lstd/lstd.h>

#include "layers/mandlebrot_layer.h"
#include "state.h"
#include "window.h"

GlobalState g_State;

int WIDTH = 1600;
int HEIGHT = 900;

// @Cleanup, we need to run this for our lstd library currently,
// but for some reason it doesn't run inside the library itself.
struct Initializer {
  Initializer() { platform_state_init(); }
};

Initializer g_Initializer = {};

bool destroy_if_it_exists_and_recreate_framebuffer() {
  if (g_State.FrameBuffer) glDeleteFramebuffers(1, ref g_State.FrameBuffer);
  if (g_State.ColorTexture) glDeleteTextures(1, ref g_State.ColorTexture);

  glGenFramebuffers(1, ref g_State.FrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, g_State.FrameBuffer);

  glGenTextures(1, ref g_State.ColorTexture);
  glBindTexture(GL_TEXTURE_2D, g_State.ColorTexture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, get_width(), get_height(), 0, GL_RGB,
               GL_UNSIGNED_BYTE, null);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         g_State.ColorTexture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Error: Failed to create framebuffer.\n");
    return false;
  }
  return true;
}

void save_frame_to_ppm(string path) {
  string header = tprint("P6\n{} {}\n255\n", get_width(), get_height());

  s64 imageSize = get_width() * get_height() * 3;
  s64 size = header.Count + imageSize;

  string contents;
  contents.Data = malloc<char>({.Count = size});
  contents.Count = size;
  defer(free(contents));

  memcpy(contents.Data, header.Data, header.Count);

  glBindTexture(GL_TEXTURE_2D, g_State.ColorTexture);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE,
                contents.Data + header.Count);

  os_write_to_file(path, contents, file_write_mode::Overwrite_Entire);
}

int run() {
  auto *window = (GLFWwindow *)create_window("Chaos", WIDTH, HEIGHT);
  if (!window) return 1;

  g_State.Window = window;

  void init_imgui();
  init_imgui();

  destroy_if_it_exists_and_recreate_framebuffer();

  // Setup layers here. The order they appear are
  // the order in which their callbacks are called.
  free(g_State.Layers);
  g_State.Layers = {&MANDLEBROT_LAYER};
  For(g_State.Layers) {
    if (!it->Init()) return 1;
  }

  ImGuiIO ref io = ImGui::GetIO();
  ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all inputs
    // to dear imgui, and hide them from your application based on those two
    // flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (io.KeysData[ImGuiKey_S].DownDuration == 0.0f && io.KeyCtrl) {
      void save_frame_to_ppm(string path);
      save_frame_to_ppm("mandelbrot.ppm");
    }

    if (io.KeysData[ImGuiKey_E].DownDuration == 0.0f && io.KeyCtrl) {
      g_State.DrawEditorUI = !g_State.DrawEditorUI;
    }

    if (g_State.DrawEditorUI) {
      void draw_imgui_menu_and_dockspace();
      draw_imgui_menu_and_dockspace();

      if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View")) {
          if (ImGui::MenuItem("Save Render", "Ctrl+S")) {
            save_frame_to_ppm("mandelbrot.ppm");
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport", null,
                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
      ImGui::PopStyleVar(1);

      ImVec2 viewportPos = ImGui::GetWindowPos();
      ImVec2 viewportSize = ImGui::GetWindowSize();
      if (!float_equal(g_State.ViewportSize.x, viewportSize.x) ||
          !float_equal(g_State.ViewportSize.y, viewportSize.y)) {
        g_State.ViewportSize = viewportSize;

        destroy_if_it_exists_and_recreate_framebuffer();
        For(g_State.Layers) { it->ViewportResized(); }
      }

      ImGui::End();

      // Draw to viewport window if we aren drawing the editor
      if (g_State.DrawEditorUI) {
        ImGui::Begin("Viewport");

        ImVec2 viewportPos = ImGui::GetWindowPos();
        ImVec2 viewportSize = ImGui::GetWindowSize();

        auto *d = ImGui::GetWindowDrawList();
        d->AddImage(
            (ImTextureID)(intptr_t)g_State.FrameBuffer, viewportPos,
            {viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y});

        ImGui::End();
      }

      For(g_State.Layers) { it->UI(); }

      ImGui::End();  // for draw_imgui_menu_and_dockspace, @Cleanup
    } else {
      // Not drawing editor UI
      int width, height;
      glfwGetWindowSize(g_State.Window, ref width, ref height);
      float fWidth = cast(float) width, fHeight = cast(float) height;

      if (!float_equal(g_State.ViewportSize.x, fWidth) ||
          !float_equal(g_State.ViewportSize.y, fHeight)) {
        g_State.ViewportSize = {fWidth, fHeight};

        destroy_if_it_exists_and_recreate_framebuffer();
        For(g_State.Layers) { it->ViewportResized(); }
      }
    }

    // Rendering
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, ref display_w, ref display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w,
                 clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_State.FrameBuffer);
    glViewport(0, 0, get_width(), get_height());
    glClear(GL_COLOR_BUFFER_BIT);

    For(g_State.Layers) { it->RenderToViewport(); }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_State.FrameBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    if (!g_State.DrawEditorUI) {
      // Draw directly to window if we aren't drawing the editor
      glBlitFramebuffer(0, 0, get_width(), get_height(), 0, 0, get_width(),
                        get_height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    free_all(TemporaryAllocator);
  }
  return 0;
}

int main() {
  constexpr s64 TEMP_SIZE = 1_MiB;
  TemporaryAllocatorData.Block = os_allocate_block(TEMP_SIZE);
  TemporaryAllocatorData.Size = TEMP_SIZE;

  tlsf_allocator_data data;

  constexpr s64 SIZE = 16_MiB;
  tlsf_allocator_add_pool(ref data, os_allocate_block(SIZE), SIZE);

  allocator alloc = {tlsf_allocator, ref data};
  PUSH_ALLOC(alloc) { return run(); }
}
