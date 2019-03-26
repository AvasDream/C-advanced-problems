CC = gcc
CFLAGS = -Wall
LDFLAGS = -static

OBJ = sysinfo.o

sysinfo: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o sysinfo $(OBJ)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<


clean:
	rm $(OBJ)
