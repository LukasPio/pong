compile:
	gcc ./src/*.c -o ./pong `sdl2-config --cflags --libs` 