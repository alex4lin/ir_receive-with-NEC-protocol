# ir_receive-with-NEC-protocol
create a ir_receive with NEC protocol on Raspberry Pi (based on pigpio)

It is a tool to decode the remote controller(NEC protocol, most popular IR protocol).


1. select the available GPIO port and connect the ir_receiver (signal pin) with it. 
https://www.raspberrypi-spy.co.uk/2012/06/simple-guide-to-the-rpi-gpio-header-and-pins/

2. Compile it by gcc.
gcc -Wall -pthread -o ir_receive ir_receive.c -lpigpio -lrt

3. Run the tool with sudo & assign a port number.
sudo ./ir_receive port


If you want to know more about NEC protocol, you can google more about it.
https://www.sbprojects.net/knowledge/ir/nec.php

It is based on the pigpio library. Refer below link.
https://github.com/joan2937/pigpio
