//cs335 Spring 2015 final project #include <iostream> #include <cstdlib> #include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Object.cpp"
#include "ppm.h"
#include "GameEngine.h"
#include "PlayState.h"

extern "C" {
#include "fonts.h"
}

int main ( int argc, char *argv[] )
{
	GameEngine game;

	// initialize the engine
	game.Init();

	// load the intro
	game.ChangeState( PlayState::Instance() );

	// main loop
	while ( game.Running() ) {
		game.Draw();
        // handle events is check keys and mouse
		game.HandleEvents();
        // update is things that need to keep running
        // playstate: movement, gettimeofday glxswapbuffers
		game.Update();

	}

	// cleanup the engine
	game.Cleanup();

	return 0;
}
