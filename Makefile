# cs335 game
CFLAGS	= -I ./include
LIB		= ./lib/fmod/libfmodex64.so


all: game tool script

game: main.cpp ppm.cpp fmod.c functions.cpp bethanyA.cpp brianS.cpp Object.cpp chadD.cpp Item.cpp Player.cpp sounds.cpp
	g++ $(CFLAGS) main.cpp ppm.cpp fmod.c functions.cpp bethanyA.cpp brianS.cpp Object.cpp chadD.cpp Item.cpp Player.cpp sounds.cpp -Wall -o game -lX11 -lGL -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 $(LIB)

tool: tedP.cpp ppm.cpp Object.cpp bethanyA.cpp chadD.cpp functions.cpp
	g++ $(CFLAGS) tedP.cpp ppm.cpp Object.cpp bethanyA.cpp chadD.cpp functions.cpp fmod.c -Wall -o tool -lX11 -lGL -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 $(LIB)

clean:
	rm -f game
	rm -f tool
	rm -f *.o

script:
	export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1
