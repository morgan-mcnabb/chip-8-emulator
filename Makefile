OBJS = main.cpp chip8.cpp
CC = g++
CFLAGS = -g -Wall
LFLAGS = -lSDL2
OBJNAME = main

.PHONY:
	all

all:
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJNAME)

clean:
	rm main *.swp *.o
