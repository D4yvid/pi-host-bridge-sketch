#include "serial.h"
#include "../message/parser.h"
#include <stdio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

message_receiver_t message_receiver;

int serial_init ()
{
    // For now, as this is a sketch project, we're going to use the pico's
    // stdio library
    if (!stdio_init_all ()) {
        return 1;
    }
}

int serial_read (char *buffer, uint64_t len)
{
    return fread (buffer, len, 1, stdin);
}

int serial_write (char *buffer, uint64_t len)
{
    fwrite (buffer, len, 1, stdout);
}

int serial_set_message_receiver (message_receiver_t receiver)
{
    message_receiver = receiver;
}

int serial_start_loop ()
{
    int code = 0;
    message_parser_t parser;
    message_t message = { 0 };
    char buffer[sizeof (raw_message_t)];

    message_parser_init (&parser, buffer);

    while (1) {
        if (-1 == (parser.size = fread (buffer, sizeof (buffer), 1, stdin))) {
            continue;
        }

        while ((code = message_parser_parse_message (&parser)) == EAGAIN) {
            if (parser.message_available) {
                memcpy (&message, parser.message_buffer, parser.message_length);

                if (message_receiver)
                    message_receiver (&message);

                message_parser_reset (&parser);
            }
        }
        
        if (parser.message_available) {
            memcpy (&message, parser.message_buffer, parser.message_length);

            if (message_receiver)
                message_receiver (&message);

            message_parser_reset (&parser);
        }
    }
}