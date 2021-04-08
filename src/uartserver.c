#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// int main(int argc, char const *argv[])
int main()
{
    /* code */

    printf("Hello, world!\n");

    int serial_fid = open("/dev/ttyUSB0", O_RDWR);

    if (serial_fid < 0)
    {
        perror("opening serial device");
        exit(EXIT_FAILURE);
    }

    char read_buffer [256];

    int bytes_read = read(serial_fid, &read_buffer, sizeof(read_buffer));

    if (bytes_read < 0)
    {
        perror("reading from serial port");
        exit(EXIT_FAILURE);
    }

    printf("Bytes read from serial device: %s", read_buffer);

    return 0;
}
