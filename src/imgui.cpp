#include "gl.h"

#include "state.h"
#include "window.h"

void init_imgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  defer_to_exit(ImGui::DestroyContext());

  ImGuiIO ref io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;           // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(g_State.Window, true);
  defer_to_exit(ImGui_ImplOpenGL3_Shutdown());

  ImGui_ImplOpenGL3_Init();
  defer_to_exit(ImGui_ImplGlfw_Shutdown());
}

void draw_imgui_menu_and_dockspace() {
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
  // window not dockable into, because it would be confusing to have two
  // docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
  // render our background and handle the pass-thru hole, so we ask Begin()
  // to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka
  // window is collapsed). This is because we want to keep our DockSpace()
  // active. If a DockSpace() is inactive, all active windows docked into it
  // will lose their parent and become undocked. We cannot preserve the
  // docking relationship between an active window and an inactive docking,
  // otherwise any change of dockspace/settings would lead to windows being
  // stuck in limbo and never being visible.
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace", null, window_flags);
  ImGui::PopStyleVar();

  ImGui::PopStyleVar(2);

  // Submit the DockSpace
  ImGuiIO ref io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Options")) {
      if (ImGui::MenuItem("Draw Editor UI", "Ctrl+E", g_State.DrawEditorUI)) {
        g_State.DrawEditorUI = !g_State.DrawEditorUI;
      }
      if (ImGui::MenuItem("Exit", "Alt+F4")) {
        glfwSetWindowShouldClose(g_State.Window, true);
      }
      ImGui::EndMenu();
    }

    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(
          "This is an awesome Raytracer written entirely (expect ImGui) "
          "from scratch in order to battle-test my C++ standard library "
          "replacement.");
      ImGui::TextUnformatted("");
      ImGui::TextUnformatted("* Controls:");
      ImGui::TextUnformatted("      * -> *");
      ImGui::TextUnformatted("      * -> *");
      ImGui::TextUnformatted("");
      ImGui::TextUnformatted("This project is under the MIT license.");
      ImGui::TextUnformatted(
          "Source code: github.com/Repertoi-e/Chaos-Course/");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }

    ImGui::EndMainMenuBar();
  }
}
