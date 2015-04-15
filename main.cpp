//cs335 Spring 2015 final project #include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Ground.cpp"
#include "Sprite.cpp"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 9001
#define GRAVITY 0.1
#define rnd()(float)rand() /(float)(RAND_MAX)

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


int main(void){
    int done=0;
    srand(time(NULL));
    initXWindows(); init_opengl();
    //declare sprite object
    Sprite sprite(50, 50, WINDOW_WIDTH/6, 700);
    Ground ground_1( 300, 10, WINDOW_WIDTH/2, 0 );

    while(!done) { //Staring Animation
        while(XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e, &sprite);
            done = check_keys(&e, &sprite);
        }
        movement(&sprite, &ground_1);
        render(&sprite, &ground_1);
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
            std::cout << " velocity y: " << sprite->getVelocityY();
            std::cout << " center y: " << sprite->getCenterY();
            // uncomment the following to disallow double jump.
            // FIXME: when double jump is disallowed, somewhere
            // velocity is -0.1, which disallows jump off any
            // ground objects. 
            //
            //if (sprite->getVelocityY() == 0) {
                sprite->setVelocityY(5);
            //}
        }
        if (key == XK_a) sprite->setVelocityX(-5);
        if (key == XK_d) sprite->setVelocityX(5);
 
        if (key == XK_z) sprite->setCameraX( sprite->getCameraX()-10 );
        if (key == XK_c) sprite->setCameraX( sprite->getCameraX()+10 );
        if (key == XK_m) {
            std::string position = getSpritePosition(sprite);
            std::cout << position + "\n";
        }


        return 0;
    }
    // control duration of jump to when key is held down
    if(e->type == KeyRelease){
        if (key == XK_a) sprite->setVelocityX(0);
        if (key == XK_d) sprite->setVelocityX(0);
    }

    return 0;
}


void movement(Sprite *sprite, Ground *ground){
    // Detect Collision
    float spriteLeft  = sprite->getCenterX() - sprite->getWidth();
    float spriteRight = sprite->getCenterX() + sprite->getWidth();
    float spriteTop   = sprite->getCenterY() + sprite->getHeight();
    float spriteDown   = sprite->getCenterY() - sprite->getHeight();

    //float groundLeft  = ground->getCenter().x - ground->getWidth();
    //float groundRight = ground->getCenter().x + ground->getWidth();
    //float groundTop   = ground->getCenter().y + ground->getWidth();
    //float groundDown  = ground->getCenter().y - ground->getWidth();
    //int collideX = 0;
    int collideY = 0;

    //float dx = boxRight - boxLeft;
    //float dy = 0;
    //int collide = 0;
    //if (spriteRight >= groundLeft
            //&& spriteLeft  <= groundRight
            //&& spriteDown  <=  groundTop
            //&& spriteTop   >=  groundDown){
        //if (sprite->velocity.y < 0) sprite->velocity.y = 0.0;
        //if(sprite->velocity.y > 0 && boxTop > groundDown) sprite->velocity.y = -5.0;
    //}
    
    sprite->setCenter( (sprite->getCenterX() + sprite->getVelocityX()), (sprite->getCenterY() + sprite->getVelocityY()));

    if ((sprite->getCenterY() - sprite->getHeight()) > 0 ){

        if (collideY != 1){
            sprite->setVelocityY( sprite->getVelocityY() - GRAVITY);
        }
    }
    else sprite->setVelocityY(0);

}

void render(Sprite *sprite, Ground *ground){
    float w, h;
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3ub(90,140,90);

    // Draw Sprite
    glPushMatrix();
    glTranslatef(sprite->getCenterX() + sprite->getCameraX(), sprite->getCenterY(), 0);
    w = sprite->getWidth();
    h = sprite->getHeight();
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd(); glPopMatrix();

    glColor3ub(0,140,90);

    //Ground
    glPushMatrix();
    glTranslatef(ground->getCenterX() + sprite->getCameraX() , ground->getCenterY(), 0);
    w = ground->getWidth();
    h = ground->getHeight();
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
    // for use in controlling screen movement.
    // the sprite should be 'left' at the beginning of the level,
    // 'mid' throughout the level, and 'right' at the end of the level.
    // retuns the position of the sprite as left, mid, or right.
}
std::string getSpritePosition(Sprite *sprite) {
    if (sprite->getCenterX() >= 0 && sprite->getCenterX() <= 300) {
        return "left";
    }
    else if (sprite->getCenterX() > 300 && sprite->getCenterX() <= 600) {
        return "mid";
    }
    else if (sprite->getCenterX() > 600 && sprite->getCenterX() <= 900) {
        return "right";
    }
    return "off screen";
}

void scrollWindow(Sprite *sprite) {
    //TODO: get current window space showing to use
    // getSpritePosition to scroll the screen, define position of sprite by
    // the position of the window.
    // use getSpritePosition to keep sprite within middle of the screen
}
