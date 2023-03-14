#pragma once

#include "common.h"

[[nodiscard("Leak")]] char *read_entire_file(const char *filePath);

enum shader_segment_type
{
    SHADER_UNKNOWN,
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COMPUTE,
    SHADER_COUNT
};

struct shader_segment
{
    char *Code = null;

    shader_segment_type Type = SHADER_UNKNOWN;
};

void free_shader_segment(shader_segment *s);

// Returns array of shader segments. Looks in filePath and splits the contents,
// looking for #vertex, #fragment, #compute special lines which separate the shaders.
// If no such line is found, then type of the returned segment is SHADER_UNKNOWN.
// Probably the caller knows what to do with it.
[[nodiscard("Leak")]] shader_segment *read_shader_file(const char *filePath);

// Create an OpenGL program object and return it
[[nodiscard]] uint create_shader(shader_segment *segments);
