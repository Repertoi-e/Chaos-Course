#pragma once

#include <lstd/common.h>

struct GLFWwindow;

struct GlobalState {
  GLFWwindow *Window = null;
  bool DrawEditorUI = true;
  ImVec2 ViewportSize;
};

extern GlobalState g_State;

inline bool float_equal(float a, float b) { return abs(a - b) < FLT_EPSILON; }
