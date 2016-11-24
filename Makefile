CFLAGS = -lpthread
INFILES = tester.c gpio.c strutils.c
OUTFILE = rpi3tester

all:
	$(CC) $(INFILES) -o $(OUTFILE) $(CFLAGS)
debug:
	$(CC) $(INFILES) -o $(OUTFILE) $(CFLAGS) -g
clean:
	rm -f $(OUTFILE)
