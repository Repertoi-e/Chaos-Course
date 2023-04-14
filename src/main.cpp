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

int run() {
  free(g_State.Layers);
  // Setup layers here. The order they appear are
  // the order in which their callbacks are called.
  g_State.Layers = {&MANDLEBROT_LAYER};

  auto *window = (GLFWwindow *)create_window("Chaos", WIDTH, HEIGHT);
  if (!window) return 1;

  g_State.Window = window;

  void init_imgui();
  init_imgui();

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

    if (io.KeysData[ImGuiKey_E].DownDuration == 0.0f && io.KeyCtrl) {
      g_State.DrawEditorUI = !g_State.DrawEditorUI;
    }

    if (g_State.DrawEditorUI) {
      void draw_imgui_menu_and_dockspace();
      draw_imgui_menu_and_dockspace();

      // If drawing editor UI then the layers should be rendered to a texture
      // into

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport", null,
                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
      ImGui::PopStyleVar(1);

      ImVec2 viewportPos = ImGui::GetWindowPos();
      ImVec2 viewportSize = ImGui::GetWindowSize();
      if (!float_equal(g_State.ViewportSize.x, viewportSize.x) ||
          !float_equal(g_State.ViewportSize.y, viewportSize.y)) {
        g_State.ViewportSize = viewportSize;

        For(g_State.Layers) { it->ViewportResized(); }
      }

      ImGui::End();

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

    For(g_State.Layers) { it->Update(); }

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
