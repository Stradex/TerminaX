CC = gcc
CFLAGS = -lncurses -I./include/ -w
SOURCES = ./src/*.c

all: compile run clean

compile: 
	$(CC) $(SOURCES) $(CFLAGS) -o rpg

run:
	./rpg

clean:
	rm rpg
