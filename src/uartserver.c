#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


// Resource
// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#baud-rate

// int main(int argc, char const *argv[])
int main()
{
    /* code */

    printf("Hello, world!\n");

    int serial_fid = open("/dev/ttyUSB0", O_RDWR);

    if (serial_fid < 0)
    {
        perror("Couldn't open serial device");
        exit(EXIT_FAILURE);
    }

    struct termios tty_device;

    if(tcgetattr(serial_fid, &tty_device) != 0) 
    {
        perror("couldn't get tty attributes");
        exit(EXIT_FAILURE);
    }

    tty_device.c_cflag &= ~PARENB; // no parity
    tty_device.c_cflag &= ~CSTOPB; // 1 stop bit
    tty_device.c_cflag &= ~CSIZE;
    tty_device.c_cflag |= CS8 | CLOCAL;
    tty_device.c_lflag = ICANON;
    tty_device.c_oflag &= ~OPOST;

    // set the baud rate to 9600 Kbaud
    const speed_t baud_rate = B9600;
    cfsetspeed(&tty_device, baud_rate);

    tcsetattr(serial_fid, TCSANOW, &tty_device);
    tcflush(serial_fid, TCOFLUSH);

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
