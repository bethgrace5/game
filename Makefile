# cs335 game
CFLAGS	= -I ./include
LIB	= ./libggfonts.so

all: game script

game: main.cpp ppm.cpp GameEngine.cpp IntroState.cpp Stage1.cpp
	g++ $(CFLAGS) main.cpp ppm.cpp GameEngine.cpp  IntroState.cpp Stage1.cpp -Wall $(LIB) -o game -lX11 -lGL -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11

clean:
	rm -f game
	rm -f *.o


script: 
	@export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1;
