//cs335 Spring 2015 final project
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 9001
#define GRAVITY 0.1
#define rnd()(float)rand() /(float)(RAND_MAX)
//TODO: get current window space showing to use
// getSpritePosition to scroll the screen, define position of sprite by
// the position of the window.

//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Structures
struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center, velocity;
};

struct Particle {
    Shape s; Vec velocity;
};

struct Sprite {
    Vec camera;
    Vec velocity;
    Vec center;
    int width;
    int height;

    Sprite(){
        //Declare a sprite shape
        width  = 50;
        height = 50;
        center.x = WINDOW_WIDTH/2;
        center.y = 50;
        center.z = 0;

        camera.x = 0; 
        camera.y = 0;

        //~Sprite(){}
    }
};
struct Ground {
    Vec center;
    int width;
    int height;

    //~Ground(){ delete [] ground;}
    Ground(){
        //Declare a ground segment
        width  = 600;
        height = 10;
        center.x = WINDOW_WIDTH/2;
        center.y = 0;
        center.z = 0;
    }
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Sprite *sprite);
int  check_keys (XEvent *e, Sprite *sprite);
void movement(Sprite *sprite, Ground *ground);
void render(Sprite *sprite, Ground *ground);
std::string getSpritePosition(Sprite *sprite);
void scrollWindow(Sprite *sprite);

// for use in controlling screen movement.
// the sprite should be 'left' at the beginning of the level,
// 'mid' throughout the level, and 'right' at the end of the level.
// retuns the position of the sprite as left, mid, or right.
std::string getSpritePosition(Sprite *sprite) {
    if (sprite->center.x >= 0 && sprite->center.x <= 300) {
        return "left";
    }
    else if (sprite->center.x > 300 && sprite->center.x <= 600) {
        return "mid";
    }
    else if (sprite->center.x > 600 && sprite->center.x <= 900) {
        return "right";
    }
    return "off screen";
}

void scrollWindow(Sprite *sprite) {
    // use getSpritePosition to keep sprite within middle of the screen
}

int main(void){
    int done=0; srand(time(NULL));
    initXWindows(); init_opengl();
    //declare sprite object
    Sprite sprite;
    Ground ground;

    while(!done) { //Staring Animation
        while(XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e, &sprite);
            done = check_keys(&e, &sprite);
        }
        movement(&sprite, &ground); render(&sprite, &ground);
        getSpritePosition(&sprite);
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows(); return 0;
}

void set_title(void){ //Set the window title bar.
    XMapWindow(dpy, win); XStoreName(dpy, win, "Box Movement");
}

void cleanupXWindows(void) { //do not change
    XDestroyWindow(dpy, win); XCloseDisplay(dpy);
}

void initXWindows(void) { //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        std::cout << "\n\tcannot connect to X server\n" << std::endl;
        //hose(&sprite);
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
        std::cout << "\n\tno appropriate visual found\n" << std::endl;
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
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void){
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
}


void check_mouse(XEvent *e, Sprite *sprite){
    static int savex = 0, savey = 0;
    //static int n = 0;
    if (e->type == ButtonRelease) { return;}
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) { //Left button was pressed
            //int y = WINDOW_HEIGHT - e->xbutton.y;
            return;
        }
        if (e->xbutton.button==3) { //Right button was pressed
            return;
        }
    }

    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x; //xpast = savex;
        savey = e->xbutton.y; //ypast = savey;
    }
}

int check_keys(XEvent *e, Sprite *sprite){
    //Was there input from the keyboard?
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        //int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_Escape) return 1;
        if (key == XK_w){
            std::cout << "JUMP!! \n";
            std::cout << " velocity y: " << sprite->velocity.y;
            std::cout << " center y: " << sprite->center.y;
            // uncomment the following to disallow double jump.
            // FIXME: when double jump is disallowed, somewhere
            // velocity is -0.1, which disallows jump off any
            // ground objects. 
            //
            //if (sprite->velocity.y == 0) {
                sprite->velocity.y = 5;
            //}
        }
        if (key == XK_a) sprite->velocity.x = -5;
        if (key == XK_d) sprite->velocity.x = 5;
 
        if (key == XK_z) sprite->camera.x -= 10;
        if (key == XK_c) sprite->camera.x += 10;
        if (key == XK_m) {
            std::string position = getSpritePosition(sprite);
            std::cout << position + "\n";
        }


        return 0;
    }
    // control duration of jump to when key is held down
    if(e->type == KeyRelease){
        if (key == XK_a) sprite->velocity.x = 0;
        if (key == XK_d) sprite->velocity.x = 0;
    }

    return 0;
}


void movement(Sprite *sprite, Ground *ground){
    // Detect Collision
    float spriteLeft  = sprite->center.x - sprite->width;
    float spriteRight = sprite->center.x + sprite->width;
    float spriteTop   = sprite->center.y + sprite->height;
    float spriteDown   = sprite->center.y - sprite->height;

    float groundLeft  = ground->center.x - ground->width;
    float groundRight = ground->center.x + ground->width;
    float groundTop   = ground->center.y + ground->height;
    float groundDown  = ground->center.y - ground->height;
    //int collideX = 0;
    int collideY = 0;

    //float dx = boxRight - boxLeft;
    //float dy = 0;
    //int collide = 0;
    if (spriteRight >= groundLeft
            && spriteLeft  <= groundRight
            && spriteDown  <=  groundTop
            && spriteTop   >=  groundDown){
        if (sprite->velocity.y < 0) sprite->velocity.y = 0.0;
        //if(sprite->velocity.y > 0 && boxTop > groundDown) sprite->velocity.y = -5.0;
    }

    sprite->center.y += sprite->velocity.y;
    sprite->center.x += sprite->velocity.x;

    if (sprite->center.y - sprite->height > 0 ){

        if (collideY != 1){
            sprite->velocity.y -= GRAVITY;
        }
    }
    else sprite->velocity.y = 0;

}

void render(Sprite *sprite, Ground *ground){
    float w, h;
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3ub(90,140,90);

    // Draw Sprite
    glPushMatrix();
    glTranslatef(sprite->center.x + sprite->camera.x, sprite->center.y, sprite->center.z);
    w = sprite->width;
    h = sprite->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd(); glPopMatrix();

    //Ground
    glPushMatrix();
    glTranslatef(ground->center.x + sprite->camera.x , ground->center.y, sprite->center.z);
    w = ground->width;
    h = ground->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd(); glPopMatrix();

    //Non-Collision Object
    /*
    glPushMatrix();
    glTranslatef(s->center.x + 600 + sprite->camera.x , s->center.y, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd(); glPopMatrix();
    */
}
