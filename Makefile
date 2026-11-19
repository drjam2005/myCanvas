LIBS = -L ./vendor/raylib/unix/lib/ -l:libraylib.a -lX11 -lm -L ./vendor/SDL/unix/lib/ -lSDL3
CFLAGS = -O0 -g3

all: ./build/main

./build/main: ./build/main.o ./build/layer.o ./build/renderer.o ./build/updater.o ./build/canvas.o
	cc $(CFLAGS) -o $@ $^ $(LIBS)

./build/main.o: ./src/main.c ./include/* | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/layer.o: ./src/structs/layer.c ./include/* | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/renderer.o: ./src/systems/renderer.c ./include/* | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/canvas.o: ./src/structs/canvas.c ./include/* | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/updater.o: ./src/systems/updater.c ./include/* | ./build/
	cc $(CFLAGS)  -c -o $@ $<

./build/:
	mkdir -p ./build/

clean:
	rm -rf ./build/
