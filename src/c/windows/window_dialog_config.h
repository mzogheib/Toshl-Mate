#pragma once
#include <pebble.h>

#define DIALOG_CONFIG_WINDOW_APP_NAME "Toshl Mate"
#define DIALOG_CONFIG_WINDOW_MESSAGE  "Set up in the\nPebble app"
#define DIALOG_CONFIG_WINDOW_AUTH 0
#define DIALOG_CONFIG_WINDOW_FAV 1

void window_dialog_config_push(int dialog_type);
