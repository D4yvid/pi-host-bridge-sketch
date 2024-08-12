#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include "parser.h"

int message_parser_init (message_parser_t *parser, char *buffer)
{
    memset (parser, 0, sizeof (*parser));

    parser->buffer = buffer;

    return 0;
}

int message_parser_parse_message (message_parser_t *parser)
{
    if (parser->message_available) {
        // We should call this again before proceeding, as we already have a
        // pending message
        return EAGAIN;
    }

    for (int i = 0; i < parser->size; i++) {
        uint8_t byte = parser->buffer[i];

        if (!parser->parsing) {
            // We can only begin the message, if we do not receive a
            // MESSAGE_BEGIN, we just skip these bytes

            if (byte != MESSAGE_BEGIN) continue;

            // We set the parser as already at the beginning of a message, 
            parser->parsing = true;
            parser->expect_end = false;
            parser->expect_length = true;

            parser->message_available = false;
            parser->message_length = 0;
            parser->message_index = 0;

            continue;
        }

        if (parser->expect_length) {
            parser->message_length = byte;
            parser->expect_length = false;

            continue;
        }

        if (parser->expect_end) {
            // We're expecting the end of the message, as we already got the
            // message length, so we're at the end of the message

            parser->parsing = false;
            parser->expect_end = false;
            parser->expect_length = false;

            parser->message_available = true;

            continue;
        }

        parser->message_buffer[parser->message_index++] = byte;

        if (parser->message_index >= parser->message_length) {
            parser->expect_end = true;
        }
    }

    return 0;
}

int message_parser_reset (message_parser_t *parser)
{
    char *buffer = parser->buffer;

    memset (parser, 0, sizeof (parser));

    parser->buffer = buffer;
    
    return 0;
}
