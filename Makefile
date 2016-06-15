CC = gcc
CFLAGS = -lpthread -std=c99
INFILES = tester.c gpio.c strutils.c
OUTFILE = tester

all:
	$(CC) $(INFILES) -o $(OUTFILE) $(CFLAGS)
debug:
	$(CC) $(INFILES) -o $(OUTFILE) $(CFLAGS) -g
clean:
	rm -f tester
