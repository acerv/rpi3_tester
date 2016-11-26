# Introduction

[![Build Status](https://travis-ci.org/acerv/rpi3tester.svg?branch=master)](https://travis-ci.org/acerv/rpi3tester)

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

## Compiling
The program must be compiled for the Raspberry PI3 platform, which is ARM.

If you are using a Debian based distribution (i.e. Ubuntu), install the tools for cross compiling:

    $ sudo apt install build-essential g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf 

To compile the program, run the following commands inside the project directory:

    $ export CC=arm-linux-gnueabihf-gcc
    $ make

Then you need to copy `rpi3tester` binary output in the Raspberry PI3 device and run it:

    $ scp ./rpi3tester pi@<raspberrypi IP>:/home/pi
    $ ssh pi@<raspberrypi IP> /home/pi/rpi3tester

## Single GPIO setup and measurement
To read/write a sinlge GPIO, use the following procedures:
* use the `input` command to setup the GPIO as input, then call the `read` command to read the GPIO status;
* use the `output` command to setup the GPIO as output, then call the `write` command to write the GPIO status.

## Frequency generator
To generate a frequency signal out of a GPIO, use the `blink` command.
The `blink` command works as follows:
* `blink start [pin] [time_ns]`: it generates a square waveform out of the given GPIO, with the specified time (in nanoseconds) between rising edges;
* `blink stop`: it stops the sqare waveform.

With these routines, it's possible to obtain a 15MHz signal (~60ns wave) maximum, but the result can be improved reducing register operations on GPIO virtual memory map (ie. knowing before the GPIO we want to use).

`nanosleep` is used to generate the square wave, but the resolution depends on the operative system. I checked it out, obtaining max 150us resolution between rising edges, when using the smallest sleep time (1ns).

## Frequency reader
To read the frequency of a sqared waveform, use the `freq` command.
The `freq` command works as follows:
* `freq start [gpio]`: it starts to read the frequency of the waveform into the specified GPIO pin
* `freq stop`: it stops the frequency read
* `freq print`: it prints the current frequency

With these routines, the program can measure signals up to 4MHz, but the result can be improved reducing register operations on GPIO virtual memory map (ie. knowing before the GPIO we want to use).
