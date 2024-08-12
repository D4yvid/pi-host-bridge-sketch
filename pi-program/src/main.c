#include <stdint.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include "config.h"
#include "types.h"
#include "_types.h"
#include "serial/serial.h"

int main ()
{
    serial_init ();

    // TODO (complete): Use second core (core 1) to run some heavy tasks, like processing I/O from the
    // serial port and parse the messages back to our main loop

    serial_start_loop ();
    
    // TODO: Process all commands sent by the host to actions in the device, for example,
    //       toggling the LED, etc.
}
