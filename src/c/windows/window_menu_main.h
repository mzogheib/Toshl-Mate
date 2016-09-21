#pragma once
#include <pebble.h>

// TODO: Dynamically allocate memory based on incoming data from phone
static char* s_menu_main_section_1_header = "Toshl Mate";
static char* s_menu_main_section_1_rows[] = {"Favourites"};
static char* s_menu_main_section_2_header = "Expenses MMM DD - MMM DD";
static char* s_menu_main_section_2_rows[] = {"Actual $000000.00", "Planned $000000.00", "Total $000000.00"};

void window_menu_main_push();
void window_menu_main_redraw();
bool window_menu_main_pushed();