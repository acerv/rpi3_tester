This is a simple test program for the Raspberry Pi 3.

This program is made to test how fast the device can be, while using GPIO operations.
There is a command-line interpreter that is used to test the device. 

=====Digital Output=====
The "blink" command generates a square wave out of the selected GPIO pin.
The commands are the following:
- blink start [pin] [time_ns]: it generates a square waveform out of the given pin,
                               which specified time (in nonoseconds) between rising edges.
- blink stop: it stops the current waveform generation

With these routines, I obtained a 15MHz signal (~60ns wave) maximum, but this result can be
improved by reducing register operations on GPIO virtual memory map (ie. by knowing alread
what's the pin the user wants to work on).

"nanosleep" and "usleep" are used to generate the square wave, but the resolution
depends on the operative system. I checked it out, obtaining max 150us
resolution between rising edges, when using the smallest possible sleep time (1ns).

=====Digital Input=====
The command prompt has the "freq" command, that can be used to measure the
frequency of a square waveform. The commands are the following:
- freq start [pin]: it starts to read the frequency of the waveform into the specified gpio pin
- freq stop: it stops the frequency read
- freq print: it prints the current frequency

With these routines, the program can measure signals up to 4MHz, but this result can be improved
by reducing register operations on GPIO virtual memory map (ie. by knowing already what's the
pin the user wants to work on).
