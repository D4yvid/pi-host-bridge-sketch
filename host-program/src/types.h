#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define internal static
#define external extern

typedef int conn_t;

// If this bit is set, the opcode is an incoming message from the host
#define OPCODE_HOST (char) 0x80

#define MAXIMUM_MESSAGE_SIZE 0xff

#define MESSAGE_BEGIN 0xde
#define MESSAGE_END 0xad

#define OPCODE_SET_LED_STATE    0x01
#define OPCODE_BUTTON_PRESSED   0x02
#define OPCODE_BUTTON_RELEASED  0x03

#define IS_VALID_OPCODE(o) ((o) == OPCODE_SET_LED_STATE || (o) == OPCODE_BUTTON_PRESSED || (o) == OPCODE_BUTTON_RELEASED)

typedef uint8_t message_opcode_t;

typedef struct raw_message_t
{
    uint8_t begin; // Begin signal

    uint8_t length;
    uint8_t data[MAXIMUM_MESSAGE_SIZE];

    uint8_t end; // End signal
} raw_message_t;

typedef struct message_t
{
    message_opcode_t opcode;

    uint8_t data[MAXIMUM_MESSAGE_SIZE - sizeof (message_opcode_t)];
} message_t;

typedef struct message_set_led_state_t
{
    // Always SET_ONBOARD_LED_STATE
    message_opcode_t opcode;

    uint8_t state;
} message_set_led_state_t;

typedef struct message_button_event_t
{
    // Always SET_ONBOARD_LED_STATE
    message_opcode_t opcode;

    uint8_t pin;
    uint8_t value;
} message_button_event_t;

#endif /** TYPES_H */