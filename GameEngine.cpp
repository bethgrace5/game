#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#include "GameEngine.h"
#include "GameState.h"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
using namespace std;

    //X Windows variables
    //Display *dpy;
    //Window win;
    //GLXContext glc;


void GameEngine::Init(GameEngine *game) {
    m_running = true; 
    //setup Xwindows
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        cout << "\n\tcannot connect to X server\n" << endl;
        //hose(&hero);
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
        cout << "\n\tno appropriate visual found\n" << endl;
        exit(EXIT_FAILURE);
    }
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask |
        PointerMotionMask |
        StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
            InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);

    // set window title
    XMapWindow(dpy, win); XStoreName(dpy, win, "Play State");
// initXWindows
//void initXWindows(void) { //do not change


}

void GameEngine::Cleanup() {
	// cleanup the all states
	while ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

    //clean up x windows
    //clean xwindows
    XDestroyWindow(dpy, win); XCloseDisplay(dpy);
//void cleanupXWindows(void) { //do not change
//}
}

void GameEngine::ChangeState(GameState* state) {
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init();
}

void GameEngine::PushState(GameState* state) {
	// pause current state
	if ( !states.empty() ) {
		states.back()->Pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init();
}

void GameEngine::PopState() {
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// resume previous state
	if ( !states.empty() ) {
		states.back()->Resume();
	}
}

void GameEngine::HandleEvents() {
	// let the state handle events
	states.back()->HandleEvents(this);
}

void GameEngine::Update() {
	// let the state update the game
	states.back()->Update(this);
}

void GameEngine::Draw() {
	// let the state draw the screen
	states.back()->Draw(this);
}
