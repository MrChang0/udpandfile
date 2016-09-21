.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g -std=c99 -O2
BIN=udpandfile
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
udpandfile:udpandfile.o
	$(CC) $(CFLAGS) $^ -o $@ -lrt
clean:
	rm -f *.o $(BIN)