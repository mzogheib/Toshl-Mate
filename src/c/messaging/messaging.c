#include "messaging.h"
#include "../windows/window_dialog_config.h"
#include "../windows/window_dialog_message.h"
#include "../windows/window_menu_main.h"
#include "../windows/window_menu_favourites.h"

void messaging_init() {
    // Register message callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open sesame
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

// Source: http://opensource.apple.com//source/Libc/Libc-167/string.subproj/strtok.c
char *
strtok(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

static struct Favourite parse_favourite(char string[]) {
    struct Favourite fav;

    // 1: id
    char *token = strtok(string, "|");
    fav.id = atoi(token);

    // 2: title
    token = strtok(NULL, "|");
    strcpy(fav.title, token);

    // 3: subtitle
    token = strtok(NULL, "|");
    strcpy(fav.subtitle, token);

    // 4: amount
    token = strtok(NULL, "|");
    strcpy(fav.amount, token);

    return fav;
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

    // Read first item
    Tuple *t = dict_read_first(iterator);

    if(t->key == MESSAGE_KEY_NO_AUTH) {
        window_dialog_config_push(DIALOG_CONFIG_WINDOW_AUTH);
    } else {
        // For all items
        while(t != NULL) {
            if(t->key == MESSAGE_KEY_EXPENSES_HEADER) {
                strcpy(s_menu_main_section_2_header, "Expenses ");
                strcat(s_menu_main_section_2_header, t->value->cstring);
            } else if(t->key == MESSAGE_KEY_EXPENSES_ACTUAL) {
                strcpy(s_menu_main_section_2_rows[0], "Actual $");
                strcat(s_menu_main_section_2_rows[0], t->value->cstring);
            } else if(t->key == MESSAGE_KEY_EXPENSES_PLANNED) {
                strcpy(s_menu_main_section_2_rows[1], "Planned $");
                strcat(s_menu_main_section_2_rows[1], t->value->cstring);
            } else if(t->key == MESSAGE_KEY_EXPENSES_TOTAL) {
                strcpy(s_menu_main_section_2_rows[2], "Total $");
                strcat(s_menu_main_section_2_rows[2], t->value->cstring);
            } else if(t->key >= MESSAGE_KEY_EXPENSE_FAVOURITE_1 && t->key <= MESSAGE_KEY_EXPENSE_FAVOURITE_5) {
                // Parse and save each favourite
                struct Favourite fav = parse_favourite(t->value->cstring);
                fav.message_key = t->key;
                s_menu_favourites_section_1_rows[fav.id] = fav;

                // TODO: Bit of a hack but ok so long as the complete list of favourites is always sent from the phone
                num_favourites = fav.id + 1;
            } else if(t->key == MESSAGE_KEY_EXPENSE_ADDED) {
                // If the "Adding expense..." dialog is still there then pop it
                if(window_dialog_message_pushed()){
                    window_stack_pop(true);
                }
                if(t->value->int32) {
                    window_dialog_message_push("Expense added!");
                } else {
                    window_dialog_message_push("Could not add expense!");
                }
            } else {
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key not recognized!");
            }

            // Look for next item
            t = dict_read_next(iterator);
        }

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

void send_dict(int msg) {
    DictionaryIterator *dict;

    // Begin dictionary
    app_message_outbox_begin(&dict);

    // Add a key-value pair for each parameter
    dict_write_int8(dict, msg, 0);

    // Send the message!
    app_message_outbox_send();
}
