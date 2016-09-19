#!/usr/bin sh
g++ main.c -o a -L/usr/local/lib -I/usr/local/include -lglew -framework OpenGL -lglfw3
g++ colored_cube.cpp -o tutorial4 -L/usr/local/lib -I/usr/local/include -I/Local/Users/john/Documents/c/ogl/ -lglew -framework OpenGL -lglfw3
