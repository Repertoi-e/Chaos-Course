#pragma once

#include <lstd/common.h>

using layer_init_callback = bool (*)();
using layer_update_callback = void (*)();
using layer_ui_callback = void (*)();
using layer_viewport_resized_callback = void (*)();

struct layer {
  layer_init_callback Init;
  layer_update_callback Update;
	layer_ui_callback UI;
  layer_viewport_resized_callback ViewportResized;
};
