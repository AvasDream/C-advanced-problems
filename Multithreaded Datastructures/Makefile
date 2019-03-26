CC=gcc
FLAGS=-lpthread -g

all: sloppy-counter simple-counter linked-list queue
	chmod +x average.sh 

simple-counter: simple-counter.c
	$(CC) $(FLAGS) -o simple-counter simple-counter.c threads.c

sloppy-counter: sloppy-counter.c
	$(CC) $(FLAGS) -o sloppy-counter sloppy-counter.c threads.c

linked-list: linked-list.c
	$(CC) $(FLAGS) -o linked-list linked-list.c threads.c

queue: queue.c
	$(CC) $(FLAGS) -o queue queue.c threads.c

clean:
	rm -rf simple-counter sloppy-counter linked-list queue
