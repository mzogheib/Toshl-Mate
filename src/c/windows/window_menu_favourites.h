#pragma once
#include <pebble.h>

// TODO: Dynamically allocate memory based on incoming data from phone

struct Favourite {
    int id;
    char title[20];
    char subtitle[20];
    char amount[10];
    uint32_t message_key;
};

static char* s_menu_favourites_section_1_header = "Expenses";
extern int num_favourites;
struct Favourite s_menu_favourites_section_1_rows[5];

void window_menu_favourites_push();
void window_menu_favourites_redraw();
bool window_menu_favourites_pushed();