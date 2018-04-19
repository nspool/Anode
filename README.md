# Anode

The beginnings of an Arkanoid clone. Written in C++ with SDL2. Use CMake to generate a project file for the development environment of your choice.

# TODO

- Fix fauly collision detection. Current collision only occurs if top-left of puck collides with a brick.
- Add game start and finish screens
- Display score
- Add sound


# BUILDNIG

Unpack the SDL2 development files and set the SDL2_DIR environment variable to point at the directory location in the same way as the previous example. Also make sure that SDL2.dll is in a folder your path. Either 32-bit or 64-bit versions appear to work fine.

As of Visual Studio 2017 it isn't necessary to explicitly generate a solution with CMake. Instead, opening the RoboNeko folder in Visual Studio 2017 will automatically create a project based on the CMake settings and allow you to build and debug from the IDE.

Alternatively using CMake from the command line to create a 32-bit solution:
 
    cd anode 
    mkdir build
    cd build
    cmake ..
	 
or 64-bit solution:

    cd anode
    mkdir build
    cd build
    cmake -A x64 ..
