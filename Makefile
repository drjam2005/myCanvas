LIBS = -L ./vendor/raylib/unix/lib/ -l:libraylib.a -lX11 -lm -L ./vendor/SDL/unix/lib/ -lSDL3
CFLAGS = -O0 -g3 -MMD -MP

PLUG = ./build/plug.o ./build/layer.o ./build/renderer.o ./build/updater.o ./build/canvas.o \
	./build/inputs.o ./build/shortcuts.o

ifdef RELEASE
all: ./build/main-release
else
all: ./build/main
endif

./build/main: ./build/main.o ./build/plug_helpers.o | ./build/libplug.so
	cc $(CFLAGS) -rdynamic -o $@ $^ $(LIBS) -ldl

./build/libplug.so: $(PLUG)
	cc $(CFLAGS) -shared -o $@.tmp $^
	mv $@.tmp $@

./build/main-release: ./build/main-release.o ./build/plug_helpers-release.o $(PLUG)
	cc $(CFLAGS) -o $@ $^ $(LIBS)

./build/main.o: ./src/main.c Makefile | ./build/
	cc $(CFLAGS) -DRELOAD -c -o $@ $<

./build/plug_helpers.o: ./src/plug/plug_helpers.c Makefile | ./build/
	cc $(CFLAGS) -DRELOAD -c -o $@ $<

./build/main-release.o: ./src/main.c Makefile | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/plug_helpers-release.o: ./src/plug/plug_helpers.c Makefile | ./build/
	cc $(CFLAGS) -c -o $@ $<

./build/plug.o: ./src/plug/plug.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/layer.o: ./src/structs/layer.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/renderer.o: ./src/systems/renderer.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/canvas.o: ./src/structs/canvas.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/updater.o: ./src/systems/updater.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/inputs.o: ./src/systems/inputs/inputs.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/shortcuts.o: ./src/systems/shortcuts.c Makefile | ./build/
	cc $(CFLAGS) -fPIC -c -o $@ $<

./build/:
	mkdir -p ./build/

-include $(wildcard ./build/*.d)

.PHONY: all clean
clean:
	rm -rf ./build/
