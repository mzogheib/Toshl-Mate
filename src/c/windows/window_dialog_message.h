#pragma once
#include <pebble.h>

#define DIALOG_MESSAGE_WINDOW_MARGIN 10

bool window_dialog_message_pushed();
void window_dialog_message_push(char *text);
