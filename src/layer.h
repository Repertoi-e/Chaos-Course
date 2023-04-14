#pragma once

#include <lstd/common.h>

using layer_init_callback = bool (*)();
using layer_render_to_viewport_callback = void (*)();
using layer_viewport_resized_callback = void (*)();
using layer_ui_callback = void (*)();

struct layer {
  layer_init_callback Init;
  layer_render_to_viewport_callback RenderToViewport;
  layer_viewport_resized_callback ViewportResized;
  layer_ui_callback UI;
};
