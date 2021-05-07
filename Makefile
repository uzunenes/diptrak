DEBUG = 0
CC = gcc
CPP = g++
CFLAGS = -Wall -Wextra -O3

OBJCC = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))
OBJCPP = $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*cpp))

LDFLAGS = -ldarknet `pkg-config --libs opencv` -ldarknet
CFLAGS += `pkg-config --cflags opencv`

ifeq ($(DEBUG), 1) 
CFLAGS += -DDEBUG
endif


all: obj dvmot


dvmot: $(OBJCPP) $(OBJCC)
	$(CC) $(OBJ) $(LDFLAGS) -o dvmot.out

obj/%.o: src/%.cpp
	$(CPP) -c $< $(CFLAGS) -o $@

obj/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@


obj:
	mkdir -p obj


clean:
	rm -f *.out obj/*.o
