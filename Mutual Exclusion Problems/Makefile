CC=gcc
FLAGS=-lpthread -g

all: reader-writer-nostarve mutex-nostarve reader-writer barrier rendezvous fork-join

fork-join: fork-join.c
	$(CC) $(FLAGS) -o fork-join fork-join.c threads.c
reader-writer: reader-writer.c
	$(CC) $(FLAGS) -o reader-writer reader-writer.c threads.c
reader-writer-nostarve: reader-writer-nostarve.c
	$(CC) $(FLAGS) -o reader-writer-nostarve reader-writer-nostarve.c threads.c
rendezvous: rendezvous.c
	$(CC) $(FLAGS) -o rendezvous rendezvous.c threads.c
barrier: barrier.c
	$(CC) $(FLAGS) -o barrier barrier.c threads.c
mutex-nostarve: mutex-nostarve.c
	$(CC) $(FLAGS) -o mutex-nostarve mutex-nostarve.c threads.c
clean:
	rm -rf reader-writer reader-writer-nostarve mutex-nostarve fork-join rendezvous barrier
