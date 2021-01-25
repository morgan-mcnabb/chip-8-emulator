OBJS = main.o chip8.o
SOURCE = main.cpp chip8.cpp
HEADER = chip8.h
OUT = main
CC = g++
FLAGS = -g -c -Wall
LFLAGS = -lSDL2

.PHONY:
	all clean

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

chip8.o: chip8.cpp
	$(CC) $(FLAGS) chip8.cpp

clean:
	rm -rf $(OBJS) $(OUT)

