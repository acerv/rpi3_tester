CC = gcc
CFLAGS = -lpthread

all:
	$(CC) tester.c gpio.c -o tester $(CFLAGS)
debug:
	$(CC) tester.c gpio.c -o tester -g $(CFLAGS)
clean:
	rm -f tester.o tester
