#include "serial.h"
#include "exception.h"
#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstring>


Serial::Serial(std::string_view portname, const speed_t speed)
{
    // Open port read-only and without controlling terminal.
    m_fd = open(portname.data(), O_RDONLY | O_NOCTTY);
    if (m_fd < 0)
        throw exception::base::Serial("Error opening", portname, ":", strerror(errno));

    // The parameters for the communication port.
    termios tty{};

    // Getting the parameters fails with the P1 serial via USB cable because it lacks a UART.
    // if (tcgetattr(m_fd, &tty) != 0)
    //     throw exception::create<exception::base::Serial>("Error getting terminal attributes:", strerror(errno));
    // Instead set sensible attributes.

    // Control mode: 8-bit characters, ignore carrier detect, enable read.
    tty.c_cflag = CS8 | CLOCAL | CREAD;

    // Input mode: Ignore framing errors and parity errors, disable break processing,
    // shut off xon/xoff control.
    tty.c_iflag = IGNPAR | IGNBRK | IXON | IXOFF | IXANY;

    // Output mode: No manipulation of data, no remapping, no delays, raw mode.
    tty.c_oflag = 0;

    // Local mode: No signaling chars, no echo, no canonical processing.
    tty.c_lflag = 0;

    // Control characters.
    // Set non-canonical mode: Reading does not wait for a new line.
    // Minimum number of characters for noncanonical read: 0 byte: Read doesn't block
    tty.c_cc[VMIN] = 0;
    // Timeout in deciseconds for noncanonical read: 0.5 seconds.
    // Note that the smart meter sends a datagram every second.
    tty.c_cc[VTIME]=5;

    // Set baud rate on output and input.
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // Discard unsent data and unread data.
    if (tcflush(m_fd, TCIOFLUSH) != 0)
        throw exception::base::Serial("Error flushing serial data:", strerror(errno));

    // Set the attributes on the serial port.
    if (tcsetattr(m_fd,TCSANOW,&tty) != 0)
        throw exception::base::Serial("Error setting attributes on serial port:", strerror(errno));
}


Serial::~Serial()
{
    if (m_fd)
        close(m_fd);
}


std::string Serial::read() const
{
    ssize_t n = 1;
    std::string data;
    while (n == 1)
    {
        char c;
        n = ::read (m_fd, &c, 1);
        if (n == 1)
            data += c;
    }
    return data;
}
