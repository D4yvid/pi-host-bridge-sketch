#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/epoll.h>
#include <linux/serial.h>
#include <arpa/inet.h>
#include <asm-generic/termbits-common.h>
#include <pthread.h>
#include "serial.h"
#include "../types.h"
#include "../message/parser.h"

/// @brief Maximum number of events for the epoll main loop
#define MAX_EVENTS 16

/// @brief The path used for this serial connection
char *device_path = NULL;

/// @brief The serial connection handle (used for reading and writing)
conn_t connection;

/// @brief The old TTY settings for the serial connection
struct termios old_tty_settings;

/// @brief The function we want to call if a serial message is received
message_receiver_t message_receiver;

/// @brief The main loop thread handle
pthread_t main_loop_handle;

internal int serial_check_device ()
{
    struct stat st;

    return stat (device_path, &st);
}

internal int serial_open_device ()
{
    if (0 != serial_check_device ()) {
        return errno;
    }

    connection = open (device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (connection < 0) {
        return errno;
    }

    if (!isatty (connection)) {
        close (connection);

        return -ENOTTY;
    }

    return 0;
}

internal int serial_setup_device ()
{
    struct termios tty;
    struct serial_struct serial;

    if (0 != tcgetattr (connection, &tty)) {
        return errno;
    }

    old_tty_settings = tty;

    // The configuration we want:
    // 115200 bps (bits per second)
    // 8 bits per character
    // no parity
    // 1 stop bit

    cfsetispeed (&tty, B115200);
    cfsetospeed (&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~(CSIZE | CSTOPB | PARENB | CRTSCTS)) | CS8 | CREAD | CLOCAL;
    tty.c_lflag = (tty.c_lflag & ~(ICANON | ECHO | IEXTEN | ECHOE | ECHONL | ISIG));
    tty.c_iflag = (tty.c_iflag & ~(IXON | IXOFF | IXANY | BRKINT | ICRNL | INPCK | ISTRIP));
    tty.c_oflag = (tty.c_oflag & ~OPOST);
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;

    if (0 != tcsetattr (connection, TCSANOW, &tty)) {
        return errno;
    }

    // Make the port be low-latency
    ioctl (connection, TIOCGSERIAL, &serial);

    serial.flags |= ASYNC_LOW_LATENCY;

    ioctl (connection, TIOCSSERIAL, &serial);

    return 0;
}

internal void *serial_main_loop_impl (void *data)
{
    int code = 0;
    message_parser_t parser;
    message_t message = { 0 };
    char buffer[sizeof (raw_message_t)];

    message_parser_init (&parser, buffer);

    while (1) {
        if (0 != serial_check_device ()) {
            fprintf (
                stderr,
                "serial_main_loop: lost connection to device %s: %s",
                device_path,
                strerror (errno)
            );
            
            break;
        }

        ssize_t size = read (connection, &buffer, sizeof (buffer));

        if (size < 0) {
            if (errno == EAGAIN) continue;

            fprintf (stderr, "read: %s\n", strerror (errno));
            continue;
        }

        parser.size = size;

        while ((code = message_parser_parse_message (&parser)) == EAGAIN) {
            if (parser.message_available) {
                memcpy (&message, parser.message_buffer, parser.message_length);

                if (message_receiver) message_receiver (&message);

                message_parser_reset (&parser);
            }
        }
        
        if (parser.message_available) {
            memcpy (&message, parser.message_buffer, parser.message_length);

            if (message_receiver) message_receiver (&message);

            message_parser_reset (&parser);
        }
    }

    return NULL;
}

int serial_init (char *p_device_path)
{
    int code = 0;

    device_path = p_device_path;

    if (0 != (code = serial_open_device ())) {
        return code;
    }

    if (0 != (code = serial_setup_device ())) {
        return code;
    }

    return code;
}

int serial_read (char *buffer, uint64_t len)
{
    // TODO: maybe make this better than a wrapper for read?
    return read (connection, buffer, len);
}

int serial_write (char *buffer, uint64_t len)
{
    // TODO: maybe make this better than a wrapper for write?
    return write (connection, buffer, len);
}

int serial_set_message_receiver (message_receiver_t receiver)
{
    message_receiver = receiver;

    return 0;
}

int serial_start_loop ()
{
    pthread_create (&main_loop_handle, NULL, &serial_main_loop_impl, NULL);
}

int serial_wait_loop_end ()
{
    pthread_join (main_loop_handle, NULL);
}

int serial_destroy ()
{
    if (0 != tcsetattr (connection, TCSANOW, &old_tty_settings)) {
        int old_errno = errno;

        close (connection);

        return old_errno;
    }

    close (connection);

    return errno;
}
