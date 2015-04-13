# cs335 game

all: game

game: boxMove.cpp
	g++ boxMove.cpp -Wall -o game -lX11 -lGL -lGLU -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11

clean:
	rm -f game
	rm -f *.o

