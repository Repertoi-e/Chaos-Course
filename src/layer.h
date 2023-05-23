#pragma once

#include <lstd/common.h>

//
// A "layer" is basically a bunch of functionality
// that gets run inside our engine. We support
// switching between them runtime so you can test
// and run multiple stuff. Think of them like "workspaces"
// for the application.
//

using layer_init_callback = bool (*)();
using layer_uninit_callback = void (*)();
using layer_render_to_viewport_callback = void (*)();
using layer_viewport_resized_callback = void (*)();
using layer_ui_callback = void (*)();

struct layer {
  layer_init_callback Init;
  layer_uninit_callback Uninit;
  layer_render_to_viewport_callback RenderToViewport;
  layer_viewport_resized_callback ViewportResized;
  layer_ui_callback UI;
};

inline const layer g_StubLayer = {[]() { return true; }, []() {}, []() {},
                                  []() {}, []() {}};