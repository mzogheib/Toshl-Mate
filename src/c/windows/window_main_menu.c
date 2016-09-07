#include "window_main_menu.h"
#include "../menu_data.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
// Used to force the back button to quit the app.
// Source: https://gist.github.com/sarfata/10574031
ClickConfigProvider previous_ccp;

#define SECTION_1 0
#define SECTION_2 1
#define NUM_MENU_SECTIONS 2
#define NUM_SECTION_ROWS_1 1
#define NUM_SECTION_ROWS_2 3

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
    switch(section_index) {
        case SECTION_1:
            return NUM_SECTION_ROWS_1;
        case SECTION_2:
            return NUM_SECTION_ROWS_2;
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
            menu_cell_basic_header_draw(ctx, cell_layer, s_main_menu_section_1_header);
            break;
        case SECTION_2:
            menu_cell_basic_header_draw(ctx, cell_layer, s_main_menu_section_2_header);
            break;
    }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    switch(cell_index->section) {
        case SECTION_1:
            menu_cell_basic_draw(ctx, cell_layer, s_main_menu_section_1_rows[cell_index->row], NULL, NULL);
            break;
        case SECTION_2:
            menu_cell_basic_draw(ctx, cell_layer, s_main_menu_section_2_rows[cell_index->row], NULL, NULL);
            break;
    }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Selected row: %d", cell_index->row);
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

bool window_main_menu_pushed() {
    return (s_main_window) ? true : false;
}

void window_main_menu_redraw() {
    menu_layer_reload_data(s_menu_layer);
}

// Define what you want to do when the back button is pressed
void back_button_handler(ClickRecognizerRef recognizer, void *context) {
    window_stack_pop_all(true);
}

// This is the new ClickConfigProvider we will set, it just calls the old one and then subscribe
// for back button events.
void new_ccp(void *context) {
    previous_ccp(context);
    window_single_click_subscribe(BUTTON_ID_BACK, back_button_handler);
}

void force_back_button(Window *window, MenuLayer *menu_layer) {
    previous_ccp = window_get_click_config_provider(window);
    window_set_click_config_provider_with_context(window, new_ccp, menu_layer);
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
    // Also bind the back button to quit the app
    force_back_button(window, s_menu_layer);

    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
    menu_layer_destroy(s_menu_layer);
}

void window_main_menu_push() {
    if(!s_main_window) {
        s_main_window = window_create();
        window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
        });
    }
    window_stack_push(s_main_window, true);
}