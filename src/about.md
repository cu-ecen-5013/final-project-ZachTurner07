# UART Server

This application sets up a TTY device that is connected to the Arduino. The default `/dev` is `ttyACM0`, but this can be overridden at compile time. 

```
#ifndef ARDUINO_SERIAL_DEVICE
#define ARDUINO_SERIAL_DEVICE "/dev/ttyACM0"
#endif


int serial_fid = open(ARDUINO_SERIAL_DEVICE, O_RDWR);

    if (serial_fid < 0)
    {
        perror("Couldn't open serial device");
        exit(EXIT_FAILURE);
    }
```

After opening the file, the serial settings are updated to match the Arduino serial device. They are as follows:
* 9600 baud
* 8 data bits
* 1 stop bit 
* no parity

```
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
```
After the device settings are updated, the file descriptor can be used to read/write.

```
int bytes_read = read(serial_fid, read_buffer, sizeof(read_buffer));

if (bytes_read < 0)
{
    perror("reading from serial port");
    exit(EXIT_FAILURE);
}
```