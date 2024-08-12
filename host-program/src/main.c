#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "serial/serial.h"
#include "types.h"

void process_serial_message (message_t *message)
{
    switch (message->opcode)
    {
    case OPCODE_BUTTON_PRESSED: {
        message_button_event_t *button = (typeof(button)) message;

        printf ("Button (pin %d) pressed\n", button->pin);
    } break;

    case OPCODE_BUTTON_RELEASED: {
        message_button_event_t *button = (typeof(button)) message;

        printf ("Button (pin %d) released\n", button->pin);
    } break;

    case OPCODE_SET_LED_STATE: {
        message_set_led_state_t *led_state = (typeof(led_state)) message;

        printf ("LED Changed state: %d", led_state->state);
    } break;
    }
}

int main (void)
{
    char *device_path = "/dev/ttyACM0";

    int code = 0;
    char c = 0;

    if (0 != (code = serial_init (device_path))) {
        fprintf (
            stderr,
            "serial_init: could not initialize device %s: %s\n",
            device_path,
            strerror (errno)
        );

        return code;
    }

    printf ("Using device /dev/ttyACM0\n");

    serial_set_message_receiver (process_serial_message);

    serial_start_loop ();

    while (true)
    {
        if (-1 == read (STDIN_FILENO, &c, 1)) {
            break;
        }

        if (c == 'l') {
            uint8_t message[] = {
                MESSAGE_BEGIN,
                0x02,
                OPCODE_HOST | OPCODE_SET_LED_STATE,
                /** LED_TOGGLE */ 2,
                MESSAGE_END,
            };

            serial_write (message, sizeof (message));
        }
    }

    serial_destroy ();
}
