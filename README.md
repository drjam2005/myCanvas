myCanvas
===
Fun little drawing side-project

> [!WARNING]
> VERY INCOMPLETE!!!


> [!WARNING]
> Dependencies (for building):
>- Raylib
>- CMake
>- C++ Compiler (ideally gcc)
>- MinGW compiler (Windows)

## Building and Running
-- Unix Systems:
```bash
	$ mkdir build
	$ cd build/
	$ cmake ..
	$ make
```
- Running:
```
	// running temporarily
	$ ./myCanvas

	// running with a file to load/save
	$ ./myCanvas fileName
```
-- Windows Systems:
```cmd
	$ cmake -S . -B build -G "MinGW MakeFiles"
	$ cmake --build build
```
- Running:
```
	// running temporarily
	$ myCanvas.exe

	// running with a file to load/save
	$ myCanvas.exe fileName
```

## BINDINGS
### brushes
- `LeftMouseDown` = `Draw`
- `Shift + LeftMouseDown + (Move Mouse Left/Right)` = `Resize Brush`
- `E` = `Toggle Eraser/Brush`
- `Ctrl+1 -> Ctrl+4` = Transparency from `25% -> 100%`

### layers
- `Ctrl+E` = `Create Layer`
- `Ctrl+W` = `Move up a layer`
- `Ctrl+S` = `Move down a layer`
- `Ctrl+Shift+W` = `Shift a layer up`
- `Ctrl+Shift+S` = `Shift a layer down`
- `Ctrl+A/D` = `Cycle between Blending Modes`

### misc
- `Ctrl+Z` = `Undo` (max of 10 undos)
- `Ctrl+Shift+Z` = `Redo`
- `Enter` = `Save`
