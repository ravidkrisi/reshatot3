# Makefile for TCP project

all: receiver sender

receiver: receiver.c
	gcc -o receiver receiver.c

sender: sender.c
	gcc -o sender sender.c

clean:
	rm -f *.o receiver sender

runs:
	./receiver

runc:
	./sender

runs-strace:
	strace -f ./receiver

runc-strace:
	strace -f ./sender