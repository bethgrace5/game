# cs335 game
CFLAGS	= -I ./include
LIB 	= ./lib/fmod/libfmodex64.so

all: game tool script

game: main.cpp ppm.cpp
	g++ $(CFLAGS) main.cpp ppm.cpp -Wall -o game -lX11 -lGL -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11

tool: editor.cpp ppm.cpp
	g++ $(CFLAGS) editor.cpp ppm.cpp -Wall -o tool -lX11 -lGL -lm -I/usr/X11R6/include -L/usr/X11R6/lib -lX11

clean:
	rm -f game
	rm -f tool
	rm -f *.o

script:
	export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/mesa/libGL.so.1
