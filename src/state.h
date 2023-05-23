#pragma once

#include <lstd/common.h>
#include <lstd/memory.h>

#include "layer.h"

struct GLFWwindow;

//
// This is the global arena allocator we use to allocate
// anything our app allocates. Currently its set up with a 16 MiB block
// in main(), but we should probably VirtualAlloc a bunch of gigabytes not
// to worry about running out of memory. This also makes sure we think
// carefully about what and where we store and avoid calling the OS
// to allocate which saves a bunch of time. Basically our dynamic allocations
// turn into pointer bumps in an arena. Additionally, 1 MiB is reserved
// for TemporaryAllocator. This allocator is another arena allocator
// which is reset at the end of every frame. It's purpose is to do fast
// dynamic allocations during frametime, which are again, just pointer
// bumps, for stuff that shouldn't persist across frame boundaries.
// Examples may include returning strings or arrays from functions
// and not worry about freeing them (e.g. convert string to utf-16 for a windows
// call). Or calculate some mesh information without having to preallocate
// an array and worry about it's size guarantees.
//
inline allocator PersistentAllocator;

constexpr s64 PERSISTENT_MEMORY_BLOCK_SIZE = 16_MiB;
constexpr s64 TEMPORARY_MEMORY_BLOCK_SIZE = 1_MiB;

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

inline GlobalState g_State;

extern int SETTINGS_WIDTH;
extern int SETTINGS_HEIGHT;

inline int get_width() {
  return g_State.ViewportSize.x == 0.0f ? SETTINGS_WIDTH
                                        : (int)g_State.ViewportSize.x;
}

inline int get_height() {
  return g_State.ViewportSize.y == 0.0f ? SETTINGS_HEIGHT
                                        : (int)g_State.ViewportSize.y;
}

inline bool float_equal(float a, float b) { return abs(a - b) < FLT_EPSILON; }
