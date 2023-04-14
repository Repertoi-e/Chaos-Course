#include "layer.h"

bool mandlebrot_layer_init();
void mandlebrot_layer_uninit();
void mandlebrot_layer_render_to_viewport();
void mandlebrot_layer_viewport_resized();
void mandlebrot_layer_ui();

inline const layer MANDLEBROT_LAYER = {
    mandlebrot_layer_init, mandlebrot_layer_uninit,
    mandlebrot_layer_render_to_viewport, mandlebrot_layer_viewport_resized,
    mandlebrot_layer_ui};
