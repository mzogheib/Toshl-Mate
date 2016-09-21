#include "window_menu_favourites.h"
#include "../windows/window_dialog_message.h"
#include "../messaging/messaging.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;

int num_favourites;

#define SECTION_1 0

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return num_favourites;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
    switch(section_index) {
        case SECTION_1:
            return num_favourites;
        default:
            return 0;
    }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    switch(section_index) {
        case SECTION_1:
            menu_cell_basic_header_draw(ctx, cell_layer, s_menu_favourites_section_1_header);
            break;
    }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    switch(cell_index->section) {
        case SECTION_1:
            menu_cell_basic_draw(ctx, cell_layer, s_menu_favourites_section_1_rows[cell_index->row].title, NULL, NULL);
            break;
    }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Selected row: %d", cell_index->row);
    send_dict(s_menu_favourites_section_1_rows[cell_index->row].message_key);
    window_dialog_message_push("Adding expense...");
}

#ifdef PBL_ROUND
static int16_t get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    if (menu_layer_is_index_selected(menu_layer, cell_index)) {
        switch (cell_index->row) {
            case 0:
                return MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT;
                break;
            default:
                return MENU_CELL_ROUND_FOCUSED_TALL_CELL_HEIGHT;
        }
    } else {
        return MENU_CELL_ROUND_UNFOCUSED_SHORT_CELL_HEIGHT;
    }
}
#endif

bool window_menu_favourites_pushed() {
    return (s_main_window) ? true : false;
}

void window_menu_favourites_redraw() {
    menu_layer_reload_data(s_menu_layer);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Create the menu layer
    s_menu_layer = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_sections = menu_get_num_sections_callback,
        .get_num_rows = menu_get_num_rows_callback,
        .get_header_height = PBL_IF_RECT_ELSE(menu_get_header_height_callback, NULL),
        .draw_header = PBL_IF_RECT_ELSE(menu_draw_header_callback, NULL),
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
        .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback, NULL),
    });

    // Bind the menu layer's click config provider to the window for interactivity
    menu_layer_set_click_config_onto_window(s_menu_layer, window);

    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
    menu_layer_destroy(s_menu_layer);
}

void window_menu_favourites_push() {
    if(!s_main_window) {
        s_main_window = window_create();
        window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
        });
    }
    window_stack_push(s_main_window, true);
}