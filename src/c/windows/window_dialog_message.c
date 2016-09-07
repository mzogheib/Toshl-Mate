/**
* Example implementation of the dialog message UI pattern.
*/

#include "window_dialog_message.h"

static Window *s_main_window;
static TextLayer *s_label_layer;
static Layer *s_background_layer;

static char string_label_message[100];

static Animation *s_appear_anim = NULL;

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
    s_appear_anim = NULL;
}

static void background_update_proc(Layer *layer, GContext *ctx) {
    graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorYellow, GColorWhite));
    graphics_fill_rect(ctx, layer_get_bounds(layer), 0, 0);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    const GEdgeInsets background_insets = {.top = bounds.size.h  /* Start hidden */};
    s_background_layer = layer_create(grect_inset(bounds, background_insets));
    layer_set_update_proc(s_background_layer, background_update_proc);
    layer_add_child(window_layer, s_background_layer);

    s_label_layer = text_layer_create(GRect(DIALOG_MESSAGE_WINDOW_MARGIN, bounds.size.h + DIALOG_MESSAGE_WINDOW_MARGIN, bounds.size.w - (2 * DIALOG_MESSAGE_WINDOW_MARGIN), bounds.size.h));
    text_layer_set_text(s_label_layer, string_label_message);
    text_layer_set_background_color(s_label_layer, GColorClear);
    text_layer_set_text_alignment(s_label_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
    text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(s_label_layer));
}

static void window_unload(Window *window) {
    layer_destroy(s_background_layer);

    text_layer_destroy(s_label_layer);

    window_destroy(window);
    s_main_window = NULL;
}

static void window_appear(Window *window) {
    if(s_appear_anim) {
        // In progress, cancel
        animation_unschedule(s_appear_anim);
    }

    GRect bounds = layer_get_bounds(window_get_root_layer(window));

    Layer *label_layer = text_layer_get_layer(s_label_layer);

    GRect start = layer_get_frame(s_background_layer);
    GRect finish = bounds;
    Animation *background_anim = (Animation*)property_animation_create_layer_frame(s_background_layer, &start, &finish);

    start = layer_get_frame(label_layer);
    const GEdgeInsets finish_insets = {
        .top = DIALOG_MESSAGE_WINDOW_MARGIN,
        .right = DIALOG_MESSAGE_WINDOW_MARGIN, .left = DIALOG_MESSAGE_WINDOW_MARGIN
    };
    finish = grect_inset(bounds, finish_insets);
    Animation *label_anim = (Animation*)property_animation_create_layer_frame(label_layer, &start, &finish);

    s_appear_anim = animation_spawn_create(background_anim, label_anim, NULL);
    animation_set_handlers(s_appear_anim, (AnimationHandlers) {
        .stopped = anim_stopped_handler
    }, NULL);
    animation_set_delay(s_appear_anim, 250);
    animation_schedule(s_appear_anim);
}

void window_dialog_message_push(char *text) {
    strcpy(string_label_message, text);

    if(!s_main_window) {
        s_main_window = window_create();
        window_set_background_color(s_main_window, GColorBlack);
        window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
            .appear = window_appear
        });
    }
    window_stack_push(s_main_window, true);
}