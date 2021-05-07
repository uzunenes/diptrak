CC = g++
CFLAGS = -Wall -Wextra -O3
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

LDFLAGS = -ldarknet `pkg-config --libs opencv` -ldarknet
CFLAGS = `pkg-config --cflags opencv`


all: obj dvmot


dvmot: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o dvmot.out

obj/%.o: src/%.cpp
	$(CC) -c $< $(CFLAGS) -o $@

obj:
	mkdir -p obj


clean:
	rm -f *.out obj/*.o
