#pragma once

#include <lstd/common.h>

// Returns an opaque pointer to GLFWwindow, if that's needed.
void *create_window(const char *title, int width, int height);