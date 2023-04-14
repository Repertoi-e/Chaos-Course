#pragma once

#include <lstd/common.h>

#include "layer.h"

struct GLFWwindow;

inline const layer g_StubLayer = {[]() { return true; }, []() {}, []() {},
                                  []() {}, []() {}};

struct GlobalState {
  GLFWwindow *Window = null;

  const layer *Layer = &g_StubLayer;

  // When we are drawing the editor
  // we render stuff to a framebuffer
  // and draw that texture in ImGUI.
  bool DrawEditorUI = true;
  GLuint FrameBuffer = 0;
  GLuint ColorTexture = 0;

  ImVec2 ViewportSize;
};

extern GlobalState g_State;

extern int WIDTH;
extern int HEIGHT;

inline int get_width() {
  if (g_State.ViewportSize.x == 0.0f) {
    return WIDTH;
  }
  return (int)g_State.ViewportSize.x;
}

inline int get_height() {
  if (g_State.ViewportSize.y == 0.0f) {
    return HEIGHT;
  }
  return (int)g_State.ViewportSize.y;
}

inline bool float_equal(float a, float b) { return abs(a - b) < FLT_EPSILON; }
