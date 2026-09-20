# Power Controller

## USB isolator

The ISKRA AM550 smart meter has a P1 port.
This port was connected with a fitting cable to a computer's USB port.
After it was connected, the smart meter no longer sent usage data to the energy provider.
The solution was to put a USB isolator between the computer port and the P1 cable.


## USB port permissions

If the power controller daemon runs as a normal user,
it cannot access the USB serial port.
Error opening /dev/ttyUSB0 : Permission denied 

The port usually is owned by group "dialout".

Add the power controller's user to that group:

$ sudo usermod -aG dialout $USER

Logout and login, or reboot.

