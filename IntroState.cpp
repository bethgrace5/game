#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#include "GameEngine.h"
#include "GameState.h"
#include "IntroState.h"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
using namespace std;

IntroState IntroState::m_IntroState;
//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


void IntroState::Init() {
    //gettimeofday(&start, NULL);

    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);
    //Set the screen background color
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    //Load images into ppm structure.
    //Create texture elements
    //must build a new set of data...
//}

    //XMapWindow(dpyIntro, winIntro); XStoreName(dpy, win, "Intro State");
}

void IntroState::Cleanup() {
    //clean xwindows
    //XDestroyWindow(dpyIntro, winIntro); XCloseDisplay(dpyIntro);
}

void IntroState::Pause() {
}

void IntroState::Resume() {
}

void IntroState::HandleEvents(GameEngine* game) {
    dpy =game->dpy;
    dpy =game->win;
    int done = 0;
        if(XPending(dpy)) {
            while(!done) {
                cout << "xpending";
                //Player User Interfaces
                XEvent e; XNextEvent(dpy, &e);
                check_mouse(&e);
                done = check_keys(&e, game);
            }
        glXSwapBuffers(dpy, win);
    }
    //game->ChangeState( PlayState::Instance() );
}

void IntroState::Update(GameEngine* game) {
}

void IntroState::Draw(GameEngine* game) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -5.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
int IntroState::check_mouse(XEvent *e) {

    static int savex = 0, savey = 0;
    //static int n = 0;
    if (e->type == ButtonRelease) { return 0;}
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) { //Left button was pressed
            //int y = WINDOW_HEIGHT - e->xbutton.y;
            return 0;
        }
        if (e->xbutton.button==3) { //Right button was pressed
            return 0;
        }
    }

    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x; //xpast = savex;
        savey = e->xbutton.y; //ypast = savey;
    }
    return 1;
}

    //check_keys
int IntroState::check_keys(XEvent *e, GameEngine *game){
    //Was there input from the keyboard?
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape){
        }
        if (key == XK_space) {
        }
        if (key == XK_q) {
            game->Quit();
        }
    }
    if(e->type == KeyRelease){
        if (key == XK_a || key == XK_Left) {
        }
        if (key == XK_d || key == XK_Right) {
        }
    }
    return 1;
}
