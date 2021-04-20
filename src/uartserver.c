#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <syslog.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#ifndef ARDUINO_SERIAL_DEVICE
#define ARDUINO_SERIAL_DEVICE "/dev/ttyACM0"
#endif

#define LOGFILE "/var/log/syslog"


//! The file descriptor of the arduino serial device
static int serial_fid;

void isConnected()
{
    syslog(LOG_DEBUG, "Attempting to connect to the Arduino");

    // check to if Arduino is connected
    const char is_alive_command[] = "a\n";

    int bytes_written = write(serial_fid, is_alive_command, sizeof(is_alive_command));

    syslog(LOG_DEBUG, "Bytes written was %d", bytes_written);
    syslog(LOG_DEBUG, "sizeof command was %ld", sizeof(is_alive_command));

    if (bytes_written < (int) sizeof(is_alive_command))
    {
        perror("writing isalive command");
        exit(EXIT_FAILURE);
    }

    int err = tcdrain(serial_fid);

    if (err < 0)
    {
        perror("tcdrain error");
        exit(EXIT_FAILURE);
    }

    // listen for response
    char read_buffer [256];
    size_t read_pos = 0;

    memset(read_buffer, 0, sizeof(read_buffer));

    bool full_packet_recieved = false;

    syslog(LOG_DEBUG, "before DO");

    int bytes_read = -1;

    do {
        bytes_read = read(serial_fid, (read_buffer + read_pos), sizeof(read_buffer));

        syslog(LOG_DEBUG, "Bytes READ was %d", bytes_written);

        if (bytes_read < 0)
        {
            if (errno == EAGAIN)
            {
                syslog(LOG_ERR, "EAGAIN returned from serial port, trying again");
            }
            else
            {
                perror("Error receiving from serial device.");
                exit(EXIT_FAILURE);
            }
        }
        else 
        {
            syslog(LOG_DEBUG, "read buffer is %s\n", read_buffer);

            // check if we got a full packet, should be non-null
            char* null_char = strchr(read_buffer, '\n');

            if (null_char == NULL)
            {
                read_pos += bytes_read;
            }
            else
            {
                // full packet was received in the buffer since a \n was found
                full_packet_recieved = true;
            }
        }
    } while (!full_packet_recieved);

    syslog(LOG_DEBUG, "Read whole word %s\n",read_buffer);

    // syslog(LOG_DEBUG, "Number of bytes read was %d",bytes_read);
    syslog(LOG_DEBUG, "Bytes read was %d\n", bytes_read);

    
    int str_length = strlen(read_buffer);

    syslog(LOG_DEBUG, "string length is %d", str_length);

    if (read_buffer[str_length-1] == '\r')
    {
        read_buffer[str_length-1] = '\0';
    }

    syslog(LOG_DEBUG, "new whole word %s\n",read_buffer);

    // TODO could fix this later
    // if (strcmp(read_buffer, "acknowledge") != 0)
    // {
    //     syslog(LOG_ERR, "strcmp response was %d", strcmp(read_buffer, "acknowledge"));
    //     syslog(LOG_ERR, "ARDUINO WAS NOT DETECTED");
    //     exit(EXIT_FAILURE);
    // }

    syslog(LOG_DEBUG, "Arduino connected");
}

// Resource
// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#baud-rate

int main(int argc, char const *argv[])
{
    /* code */
    bool run_as_daemon = false;

    if (argc > 1)
    {
        // command line argument included
        if (strcmp("-d", argv[1]) == 0)
        {
            // set up the daemon
            printf("Setting up as a daemon\n");
            run_as_daemon = true;
        }
    }

    // open the log
    openlog(LOGFILE, 0, LOG_USER);

    syslog(LOG_DEBUG, "Log opened for UART server");

    serial_fid = open(ARDUINO_SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_SYNC);

    syslog(LOG_DEBUG, "serial device fid is %d", serial_fid);

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

    if (run_as_daemon)
    {
        // kick off a child process as a daemon to run the socket server
        daemon(0, 0);
    }

    isConnected();

    // listen for response
    char read_buffer [256];
    memset(read_buffer, 0, sizeof(read_buffer));

    // check to if Arduino is connected
    const char start_processing[] = "c";

    int bytes_written = write(serial_fid, start_processing, sizeof(start_processing));

    syslog(LOG_DEBUG, "Bytes written was %d", bytes_written);

    if (bytes_written < (int) sizeof(start_processing))
    {
        perror("writing isalive command");
        exit(EXIT_FAILURE);
    }

    do 
    {
        int bytes_read = read(serial_fid, read_buffer, sizeof(read_buffer));

        if (bytes_read < 0)
        {
            perror("reading from serial port");
            exit(EXIT_FAILURE);
        }

        printf("Bytes read from serial device: %s\n", read_buffer);
        syslog(LOG_DEBUG, "Bytes read from serial device: %s\n", read_buffer);

        // TODO add GPIO to indicate to user

    } while (true);
    
    return 0;
}
