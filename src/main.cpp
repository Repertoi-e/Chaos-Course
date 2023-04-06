// Emedded font
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
// clang-format off
#include <glad/glad.h>
#include <glfw/glfw3.h>
// clang-format on
#include <imgui.h>
#include <lstd/lstd.h>

#include "imgui/Roboto-Regular.embed"
#include "layers/mandlebrot_layer.h"
#include "window.h"

int WIDTH = 1920;
int HEIGHT = 1080;

using layer_init_callback = bool (*)();
using layer_update_callback = void (*)();
using layer_ui_callback = void (*)();

array<layer_init_callback> g_LayersInit;
array<layer_update_callback> g_LayersUpdate;
array<layer_ui_callback> g_LayersUI;

void init_imgui(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  defer_to_exit(ImGui::DestroyContext());

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable
  // Docking io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable
  // Multi-Viewport / Platform Windows io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  defer_to_exit(ImGui_ImplOpenGL3_Shutdown());

  ImGui_ImplOpenGL3_Init();
  defer_to_exit(ImGui_ImplGlfw_Shutdown());

  // Load default font
  // ImFontConfig fontConfig;
  // fontConfig.FontDataOwnedByAtlas = false;
  // ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF((void
  // *)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
  // io.FontDefault = robotoFont;
}

// @Cleanup, we need to run this for our lstd library currently,
// but for some reason it doesn't run inside the library itself.
struct Initializer {
  Initializer() { platform_state_init(); }
};

Initializer g_Initializer = {};

int run() {
  // Setup layers here! The order they appear are
  // the order in which they are called.
  g_LayersInit = {mandlebrot_layer_init};
  g_LayersUpdate = {mandlebrot_layer_update};
  g_LayersUI = {mandlebrot_layer_ui};

  auto *window = (GLFWwindow *)create_window("Chaos", WIDTH, HEIGHT);
  if (!window) return 1;

  init_imgui(window);

  For(g_LayersInit) {
    if (!it()) return 1;
  }

  ImGuiIO &io = ImGui::GetIO();
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

    {
      /*
      static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

      // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
      window not dockable into,
      // because it would be confusing to have two docking targets within each
      others. ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking; const
      ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove; window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

      // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
      render our background
      // and handle the pass-thru hole, so we ask Begin() to not render a
      background. if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
          window_flags |= ImGuiWindowFlags_NoBackground;

      // Important: note that we proceed even if Begin() returns false (aka
      window is collapsed).
      // This is because we want to keep our DockSpace() active. If a
      DockSpace() is inactive,
      // all active windows docked into it will lose their parent and become
      undocked.
      // We cannot preserve the docking relationship between an active window
      and an inactive docking, otherwise
      // any change of dockspace/settings would lead to windows being stuck in
      limbo and never being visible.
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("DockSpace Demo", null, window_flags);
      ImGui::PopStyleVar();

      ImGui::PopStyleVar(2);

      // Submit the DockSpace
      ImGuiIO &io = ImGui::GetIO();
      if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
      {
          ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
          ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
      }*/

      For(g_LayersUI) { it(); }

      // ImGui::End();
    }

    // Rendering
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w,
                 clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    For(g_LayersUpdate) { it(); }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    /*if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }*/

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

  constexpr s64 SIZE = 1_MiB;
  tlsf_allocator_add_pool(&data, os_allocate_block(SIZE), SIZE);

  allocator alloc = {tlsf_allocator, &data};
  PUSH_ALLOC(alloc) { return run(); }
}
