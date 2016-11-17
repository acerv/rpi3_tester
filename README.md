# Introduction
This is a simple test program for the Raspberry Pi 3, that can be use to measure the efficency of the GPIOs.
It can also be used to read input frequency of a waveform and the generate frequency signals.

The program command-line looks like this:

    Use one of the following commands:
    - input [gpio]
    - output [gpio]
    - read [gpio]
    - write [gpio]
    - blink start [gpio] [time_ns]
    - blink stop [gpio]
    - freq start [gpio]
    - freq stop
    - freq print
    - exit
    
    cmd> 

# Single GPIO setup and measurement
To read/write a sinlge GPIO, this must be set to work with input/output mode.
Use the `input` command to setup the GPIO as input, then use the `read` command.
Use the `output` command to setup the GPIO as output, then use the `write` command.

# Frequency generator
To generate a frequency out of a GPIO, 
The `blink` command usage is the following:
* `blink start [pin] [time_ns]`: it generates a square waveform out of the given GPIO, with the specified time (in nanoseconds) between rising edges;
* `blink stop`: it stops the sqare waveform.

With these routines, it's possible to obtain a 15MHz signal (~60ns wave) maximum, but the result can be improved by reducing register operations on GPIO virtual memory map (ie. knowing before the GPIO we want to use).

`nanosleep` is used to generate the square wave, but the resolution depends on the operative system. I checked it out, obtaining max 150us resolution between rising edges, when using the smallest sleep time (1ns).

# Frequency reader
To read the frequency of a sqared waveform, use the `freq` command.
The `freq` command usage is the following:
* `freq start [gpio]`: it starts to read the frequency of the waveform into the specified GPIO pin
* `freq stop`: it stops the frequency read
* `freq print`: it prints the current frequency

With these routines, the program can measure signals up to 4MHz, but the result can be improved by reducing register operations on GPIO virtual memory map (ie. knowing before the GPIO we want to use).
