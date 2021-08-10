PROJECT_NAME = diptrak
CC = gcc
CPP = g++
CFLAGS = -Wall -Wextra -O3

OBJCC = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))
OBJCPP = $(patsubst src/%.cpp, obj/%.o, $(wildcard src/*cpp))

LDFLAGS = -ldarknet `pkg-config --libs opencv`
CFLAGS += `pkg-config --cflags opencv`


all: obj $(PROJECT_NAME)


$(PROJECT_NAME): $(OBJCC) $(OBJCPP)
    $(CPP) $(OBJCC) $(OBJCPP) $(LDFLAGS) -o $(PROJECT_NAME).out


obj/%.o: src/%.c
    $(CC) -c $< $(CFLAGS) -o $@

obj/%.o: src/%.cpp
    $(CPP) -c $< $(CFLAGS) -o $@


obj:
    mkdir -p obj


clean:
    rm -f *.out obj/*.o