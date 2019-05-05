CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -g 
DEBUG = -g
TARGETS = client

# Mark the default target to run (otherwise make will select the first target in the file)
.DEFAULT: all
# Mark targets as not generating output files (ensure the targets will always run)
.PHONY: all debug clean

all: $(TARGETS) 


client: client.h client.c
	$(CC) $(CFLAGS) -o client -lncurses client.c


