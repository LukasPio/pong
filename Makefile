compile:
	gcc ./src/*.c -o ./pong `sdl2-config --cflags --libs` -lSDL2_ttf