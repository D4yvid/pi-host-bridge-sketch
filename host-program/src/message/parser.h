#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "../types.h"

typedef struct message_parser_t {
    char *buffer;
    uint32_t size;

    uint8_t message_length, message_index;
    uint8_t message_buffer[MAXIMUM_MESSAGE_SIZE];

    bool message_available;

    bool parsing, expect_end, expect_length;
} message_parser_t;

int message_parser_init (message_parser_t *parser, char *buffer);

int message_parser_parse_message (message_parser_t *parser);

int message_parser_reset (message_parser_t *parser);

#endif /** MESSAGE_PARSER_H */