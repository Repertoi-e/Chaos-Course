#pragma once

#include <lstd/array.h>
#include <lstd/common.h>
#include <lstd/string.h>

#include "asset.h"

enum shader_segment_type {
  SHADER_UNKNOWN,
  SHADER_VERTEX,
  SHADER_FRAGMENT,
  SHADER_COMPUTE,
  SHADER_COUNT
};

struct shader_segment {
  string Code;
  shader_segment_type Type = SHADER_UNKNOWN;
};

void free(shader_segment ref s);

// Returns array of shader segments. Looks in filePath and splits the contents,
// looking for #vertex, #fragment, #compute special lines which separate the
// shaders. If no such line is found, then type of the returned segment is
// SHADER_UNKNOWN. Probably the caller knows what to do with it.
mark_as_leak array<shader_segment> read_shader_file(string filePath);

// Create an OpenGL program object and return it
mark_as_leak u32 create_shader(array<shader_segment> segments);

struct shader : public asset {
  shader() { Type = Shader; }

  u32 Program;
};

shader get_shader_from_key(asset_key key);