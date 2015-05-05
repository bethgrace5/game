//cs335 Spring 2015 final project #include <iostream> #include <cstdlib> #include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Object.cpp"
#include "ppm.h"
#include "GameEngine.h"
#include "Stage1.h"

extern "C" {
#include "fonts.h"
}

int main ( int argc, char *argv[] )
{
	GameEngine game;
	// initialize the engine
	game.Init(&game);
	// set initial state
	game.ChangeState( Stage1::Instance() );

	// main loop
	while ( game.Running() ) {
        // handle events is check keys and mouse
		game.HandleEvents();
        // update is things that need to keep running
        // playstate: movement, gettimeofday glxswapbuffers
		game.Update();
        game.Draw();
	}

	// cleanup the engine
	game.Cleanup();

	return 0;
}
