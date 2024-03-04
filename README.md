# README FILE
# Original Project By: Trevor Pope, Micheal Callahan, and Duc Vo
# Modified By: Spencer Meren, Samson Becenti

## Building

To build this project, execute the following command from the root directory:

	g++ -I/usr/include/freetype2 main.cpp glad.c -o main -lglfw -lGLU -lGL -ldl -lSOIL -lfreetype

A different path to `freetype2` will likely be required, depending on your operating system.

## Run the program

To execute the program, simply run the `main` executable with the following command:

	./main


## Dependencies

1) OpenGL

2) GLFW

3) GLU

4) libdl

5) SOIL

6) freetype

	`sudo apt-get install libfreetype-dev libfreetype6 libfreetype6-dev`

Last updated: March 2, 2024