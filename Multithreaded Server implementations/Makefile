CC=gcc
FLAGS=-lpthread -g -lrt

all: file-server-signals file-server-async file-server select-server simple-server
	echo "Its wednesday my dudes!" > test

simple-server: simple-server.c
	$(CC) $(FLAGS) -o simple-server simple-server.c

select-server: select-server.c
	$(CC) $(FLAGS) -o select-server select-server.c

file-server: file-server.c file-server-client.c
	$(CC) $(FLAGS) -o file-server file-server.c
	$(CC) $(FLAGS) -o file-server-client file-server-client.c

file-server-async: file-server-async.c
	$(CC) $(FLAGS) -o file-server-async file-server-async.c
	$(CC) $(FLAGS) -o file-server-client file-server-client.c

file-server-signals: file-server-signals.c
	$(CC) $(FLAGS) -o file-server-signals file-server-signals.c
	$(CC) $(FLAGS) -o file-server-client file-server-client.c
clean:
	rm -rf simple-server select-server file-server file-server-signals file-server-async file-server-client


# clear && make clean && make && echo -e "*********\n\n\n" && ./file-server
