#include "layer.h"

bool mandlebrot_layer_init();
void mandlebrot_layer_update();
void mandlebrot_layer_ui();
void mandlebrot_layer_viewport_resized();

inline const layer MANDLEBROT_LAYER = {
    mandlebrot_layer_init, mandlebrot_layer_update, mandlebrot_layer_ui,
    mandlebrot_layer_viewport_resized};
