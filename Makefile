LIBS = -L ./vendor/raylib/unix/lib/ -l:libraylib.a -lX11 -lm -L ./vendor/SDL/unix/lib/ -lSDL3

all: ./build/main

./build/main: ./build/main.o
	cc -o ./build/main ./build/main.o $(LIBS)

./build/main.o: ./src/main.c | ./build/
	cc -c -o ./build/main.o ./src/main.c

./build/:
	mkdir -p ./build/
