#pragma once

#include <lstd/common.h>

struct GLFWwindow;
struct layer;

struct GlobalState {
  array<const layer *> Layers;
  GLFWwindow *Window = null;
  bool DrawEditorUI = true;
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
