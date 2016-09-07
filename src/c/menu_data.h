#pragma once
#include <pebble.h>

static char* s_main_menu_section_1_header = "Toshl Mate";
static char* s_main_menu_section_1_rows[] = {"Favourites"};
static char* s_main_menu_section_2_header = "Expenses MMM DD - MMM DD";
// Hard coded lengths for now. Should be dynamic based on incoming message
static char* s_main_menu_section_2_rows[] = {"Actual $000000.00", "Planned $000000.00", "Total $000000.00"};
