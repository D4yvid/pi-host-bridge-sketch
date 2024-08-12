#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include "../types.h"

typedef void (*message_receiver_t)(message_t *message);

int serial_init();

int serial_read(char *buffer, uint64_t len);
int serial_write(char *buffer, uint64_t len);

int serial_set_message_receiver (message_receiver_t receiver);

int serial_start_loop ();

int serial_destroy ();

#endif /** SERIAL_H */