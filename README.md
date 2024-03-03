# README FILE
# Original Project By: Trevor Pope, Micheal Callahan, and Duc Vo
# Modified By: Spencer Meren, Samson Becenti

## Building

To build this project, execute the following command from the root directory:

	g++ -I/usr/include/freetype2 main.cpp glad.c -o main -lglfw -lGLU -lGL -ldl -lSOIL -lfreetype

A different path to `freetype2` will likely be required, depending on your operating system.

## Run the program

To execute the program, simply run the `main` executable with the following command.


## Dependencies

OpenGL, GLFW, GLU, libdl, SOIL, freetype