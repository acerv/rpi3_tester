This is a simple test program for the Raspberry Pi 3.

It's mainly focused on digital up/down operations, since
my idea was to check how fast the device can generate digital
triggering signals.

There is a command-line interpreter which can be used to test the device, 
by selecting the input/output state of the gpio pins and asking for
writing/reading/blink led operations.

The most important feature for testing the device speed is the "blink" command,
that generates a square wave out of the selected gpio pin.

Experimentally, I obtained a 15MHz signal (~60ns wave) when not using
any sleep between up & down state change.

"nanosleep" is used to generate the square wave, but the resolution
depends on the operative system. I checked it out, obtaining max 150us
resolution.
