# Anode

The beginnings of an Arkanoid clone.
Written in C++ with SDL2.
Use CMake to generate a project file for the development environment of your choice.

![Screenshot](Anode.png?raw=true "Screenshot showing Anode, an incomplete Arkanoid clone")

## TODO

- Fix fauly collision detection. Current collision only occurs if top-left of puck collides with a brick.
- Add game start and finish screens
- Display score
- Add sound

### Getting Started

Anode uses CMake

### Prerequisites
Unpack the SDL2 and SDL2_Image development files and add their respective DLLs to your path.

### Building

On Windows, generate the project file:

````
cd Anode && mkdir build && cd build
cmake -A x64 -DSDL2_DIR=<PATH_TO_SDL2> -SDL2_IMAGE_DIR=<PATH_TO_SDL2_IMAGE> ..
```

If you are on macOS and have the SDL2 frameworks installed then the path variables can be omitted.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.