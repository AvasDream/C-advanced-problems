CC=gcc
src=$(wildcard *.c)
obj=$(src:.c=.o)

easy_server: $(obj)
	$(CC) -static -o $@ $^

.PHONY: all clean

all: easy_server
clean:
	rm -f $(obj)