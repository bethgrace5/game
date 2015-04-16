//cs335 Spring 2015 final project #include <iostream> #include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Ground.cpp"
#include "Sprite.cpp"


#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 2000
#define GRAVITY 0.2
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

//hero sprite globals
int didJump = 2;
int collideX = 0;
int collideY = 0;
int wantToJump = 0;

//window globals
int scrollRight = 0;
int scrollLeft = 0;

int main(void){
    std::string currentPosition;
    std::string previousPosition;
    int done=0;
    srand(time(NULL));
    initXWindows(); init_opengl();
    //declare sprite object
    Sprite sprite(50, 50, WINDOW_WIDTH/6, 500);
    Ground ground_1( 300, 10, WINDOW_WIDTH/2, 0 );

    while(!done) { //Staring Animation
        while(XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e, &sprite);
            done = check_keys(&e, &sprite);
        }
        currentPosition = getSpritePosition(&sprite);
        movement(&sprite, &ground_1);
        render(&sprite, &ground_1);
        scrollWindow(&sprite);

        // game just started, character is in the left segment.
        /*
        if (currentPosition == "left") {
        }
        else if (currentPosition == "right" && previousPosition != "right") {
            previousPosition = "right";
            std::cout<<"changed position to right";
            scrollWindow(&sprite, "right");
        }
        else {
            previousPosition = currentPosition;

        }
        //currentPosition = getSpritePosition(&sprite);
        */


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
        if (key == XK_Escape) return 1;
        if (key == XK_w && didJump<2){
            std::cout << "JUMP!! \n";
            std::cout << " velocity x: " << sprite->getVelocityX();
            std::cout << " velocity y: " << sprite->getVelocityY();
            std::cout << " center x: " << sprite->getCenterX();
            std::cout << " center y: " << sprite->getCenterY();
            // TODO: disallow jumping while already in the air.
            didJump++;
			wantToJump=1;	
//            sprite->setVelocityY(5);
        }
        if (key == XK_a) {
            sprite->setVelocityX(-5);
            if (getSpritePosition(sprite) == "left") {
                // uncomment to scroll both directions
                 scrollRight = 1;
            }
        }
        if (key == XK_d) {
            sprite->setVelocityX(5);
            if (getSpritePosition(sprite) == "right") {
                scrollLeft = 1;
            }
        }
 
        if (key == XK_z) {
            sprite->setCameraX( sprite->getCameraX()-10 );
        }
        if (key == XK_c) sprite->setCameraX( sprite->getCameraX()+10 );
        if (key == XK_m) {
            std::string position = getSpritePosition(sprite);
            std::cout << position + "\n";
        }


        return 0;
    }
    if(e->type == KeyRelease){
        if (key == XK_a) {
            sprite->setVelocityX(0);
            scrollRight = 0;
        }
        if (key == XK_d) {
            sprite->setVelocityX(0);
            scrollLeft = 0;
        }
    }

    return 0;
}


void movement(Sprite *sprite, Ground *ground){
    // Detect Collision
	float spriteCenterX = sprite->getCenterX();
	float spriteCenterY = sprite->getCenterY();
	float spriteVelocityX = sprite->getVelocityX();
	float spriteVelocityY = sprite->getVelocityY();
	float spriteWidth = sprite->getWidth();
	float spriteHeight = sprite->getHeight();
    float spriteLeft  = spriteCenterX - spriteWidth;
    float spriteRight = spriteCenterX + spriteWidth;
    float spriteTop   = spriteCenterY + spriteHeight;
    float spriteDown  = spriteCenterY - spriteHeight;

	float groundCenterX = ground->getCenterX();
	float groundCenterY = ground->getCenterY();
    float groundLeft  = groundCenterX - ground->getWidth();
    float groundRight = groundCenterX + ground->getWidth();
    float groundTop   = groundCenterY + ground->getWidth();
    float groundDown  = groundCenterY - ground->getWidth();

    //float dx = boxRight - boxLeft;
    //float dy = 0;
    //int collide = 0;
    if (spriteRight >= groundLeft
    	&& spriteLeft  <= groundRight
        && spriteDown  <=  groundTop
        && spriteDown   >=  groundDown){
		collideY = 1;
		printf("collideY : sprite (L %1.1f, R %1.1f, T %1.1f, D %1.1f) ground (L %1.1f, R %1.1f, T %1.1f, D %1.1f) \n", spriteLeft, spriteRight, spriteTop, spriteDown, groundLeft, groundRight, groundTop, groundDown);
	}

/*
    if (spriteRight >= groundLeft
    	&& spriteLeft  <= groundRight
        && spriteTop  <=  groundTop
        && spriteTop   >=  groundDown){
		spriteVelocityY *= -2.0;
	}     
*/
    
 //    if ((spriteCenterY - sprite->getHeight()) > 0 ){

	if (collideY != 1){
		if (collideX != 1){
			sprite->setVelocityY(spriteVelocityY - GRAVITY);
	        sprite->setCenter( 
				spriteCenterX + spriteVelocityX , 
				spriteCenterY + spriteVelocityY 
			);
		}
		else{ // collision with X

		}
	}
    else{ // collision with Y (floor only)
		sprite->setCenter( spriteCenterX , groundTop + .1 + ((spriteTop-spriteDown)/2) );
		sprite->setVelocityY(0);
		didJump = 0;
		collideY = 0;
	}
	if (wantToJump){
		sprite->setCenter(spriteCenterX, spriteCenterY - spriteVelocityY);
    	sprite->setVelocityY(5);
		wantToJump = 0;
	}


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

    if (scrollRight) {
        sprite->setCameraX( sprite->getCameraX()+4 );
    }
    if (scrollLeft) {
        sprite->setCameraX( sprite->getCameraX()-4 );
    }

}
