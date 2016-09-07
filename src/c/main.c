#include <pebble.h>
#include "messaging/messaging.h"
#include "windows/window_dialog_message.h"
#include "windows/window_main_menu.h"


static Window *s_main_window;

static void init(void) {
    window_dialog_message_push("Connecting...");
    messaging_init();
}

static void deinit(void) {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
