#include "messaging.h"
#include "../windows/window_dialog_config.h"
#include "../windows/window_menu_main.h"
#include "../menu_data.h"

void messaging_init() {
    // Register message callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open sesame
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

    Tuple *no_auth = dict_find(iterator, MESSAGE_KEY_NO_AUTH);
    if(no_auth) {
        window_dialog_config_push();
    } else {
        Tuple *expenses_header = dict_find(iterator, MESSAGE_KEY_EXPENSES_HEADER);
        if(expenses_header) {
            strcpy(s_menu_main_section_2_header, "Expenses ");
            strcat(s_menu_main_section_2_header, expenses_header->value->cstring);
        }

        Tuple *expenses_actual = dict_find(iterator, MESSAGE_KEY_EXPENSES_ACTUAL);
        if(expenses_actual) {
            strcpy(s_menu_main_section_2_rows[0], "Actual $");
            strcat(s_menu_main_section_2_rows[0], expenses_actual->value->cstring);
        }

        Tuple *expenses_planned = dict_find(iterator, MESSAGE_KEY_EXPENSES_PLANNED);
        if(expenses_planned) {
            strcpy(s_menu_main_section_2_rows[1], "Planned $");
            strcat(s_menu_main_section_2_rows[1], expenses_planned->value->cstring);
        }

        Tuple *expenses_total = dict_find(iterator, MESSAGE_KEY_EXPENSES_TOTAL);
        if(expenses_total) {
            strcpy(s_menu_main_section_2_rows[2], "Total $");
            strcat(s_menu_main_section_2_rows[2], expenses_total->value->cstring);
        }

        // Future messages:
        // Expense added
        // Expense could not be added

        if(window_menu_main_pushed()) {
            window_menu_main_redraw();
        } else {
            window_menu_main_push();
        }
    }
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! %d %d %d", reason, APP_MSG_SEND_TIMEOUT, APP_MSG_SEND_REJECTED);

}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
