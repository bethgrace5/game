#include <iomanip>
#include <iostream>
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
#include "functions.h"
#include <sstream>
#include <algorithm>
#include "Sprite.cpp"
#include "Player.h"

//#include "fastFont.h"
#include "tedP.cpp"

//Platforms/Grounds
#include "cdanner.cpp"

//Enemies
#include "bsingenstrew.cpp"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define WINDOW_HALF_WIDTH  WINDOW_WIDTH/2
#define WINDOW_HALF_HEIGHT WINDOW_HEIGHT/2
#define LEVEL_WIDTH 10000
#define MAX_HEIGHT 1200
#define MAX_GROUNDS 100
#define MAX_ENEMIES 100
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]

// 1 for quick load, 0 for slow load with menu images
#define QUICK_LOAD_TIME 1

#define MAX_BACKGROUND_BITS 6000
#define HERO_START_X 150
#define HERO_START_Y 350

using namespace std;

typedef double Vec[3];

struct bgBit {
    Vec pos;
    Vec lastpos;
    Vec vel;
    struct bgBit *next;
    struct bgBit *prev;
    const char* n;
};

struct Bullet {
    Vec pos;
    Vec vel;
    float color[3];
    struct timespec time;
    struct Bullet *prev;
    struct Bullet *next;
    Bullet() {
        prev = NULL;
        next = NULL;
    }
};
/*
int diff_ms (timeval t1, timeval t2) {
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}

template <typename T>
string itos (T Number) {
    stringstream ss;
    ss << Number;
    return (ss.str());
}
*/
//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Hero Globals
int didJump=0;
int lives=3;
int life=100;
// last facing 0 means facing forward(right), 1 means backward(left)
int lastFacing=0;
double h_right, h_left, h_top, h_bottom;
timeval seqStart, seqEnd; // hero's sprite index
timeval fireStart, fireEnd; // hero's fire rate timer
timeval frameStart, frameEnd; // menu
int frameIndex=0;

//Game Globals
string str = "";
bgBit *bitHead = NULL;
Bullet *bhead = NULL;
Enemy *enemies[MAX_ENEMIES];
Object *hero; // Class Player
Platform *grounds[MAX_GROUNDS];
int bg, bullets, grounds_length, enemies_length,  i, j, level=-1;
int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;
int fail=0;
int interval=120;
double g_left, g_right, g_top, g_bottom;
int quit=0;

//Images and Textures
Ppmimage *heroImage=NULL;
GLuint heroTexture;
Ppmimage *menuImage[40];
GLuint menuTexture[40];
Ppmimage *initImages[32];
GLuint initTextures[32];
Ppmimage *computerScreenImages[26];
GLuint computerScreenTextures[26];

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e);
int  check_keys (XEvent *e);
void movement(void);
void render(void);
void moveWindow(void);
void renderBackground(void);
void cleanup_background(void);
void renderGrounds(int x, int y);
void renderBullets(int x, int y);
void deleteBullet(Bullet *node);
void deleteEnemy(int ind);
void renderEnemies(int x, int y);
void renderHero(int x, int y);
void renderInitMenu();
void renderComputerScreenMenu();
void makePlatform(int w, int h, int x, int y); 
void makeEnemy(int w, int h, Object *ground, int type); 
Object createAI( int w, int h, Object *ground);

void groundCollide(Object *obj, Object *ground);
bool detectCollide(Object *obj, Object *ground);
bool bulletCollide(Bullet *b, Object *obj);

bool inWindow(Object &obj) {
    return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
                obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or
            (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
             obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
}

int main(void) {
    initXWindows(); init_opengl();

    //declare hero object
    hero = new Object(46, 48, HERO_START_X, HERO_START_Y);
    hero->setTop(40);
    hero->setBottom(-44);
    hero->setLeft(-26);
    hero->setRight(26);

    level = 1;
    frameIndex = 0;

    if(QUICK_LOAD_TIME) {
        level = 1;
    }

    while (!quit) { //Staring Animation
        while (XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            //check_mouse(&e);
            quit = check_keys(&e);
        }
        if (level == -1) {
            renderInitMenu();
            //check_mouse(&e);
            // level = check_keys_menu(&e);
        }
        else if (level == 0) {
            renderComputerScreenMenu();
        }
        else {
            movement();
            render();
            moveWindow();
        }
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows(); return 0;
    //glDeleteTextures(1, &heroTexture);
    //glDeleteTextures(1, &menuTexture);
}

void set_title (void) { //Set the window title bar.
    XMapWindow(dpy, win); XStoreName(dpy, win, "Revenge of the Code");
}

void cleanupXWindows (void) { //do not change
    cleanup_background();
    XDestroyWindow(dpy, win); XCloseDisplay(dpy);
}

void initXWindows (void) { //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        cout << "\n\tcannot connect to X server\n" << endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
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
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl (void) {
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
    initFastFont();

    //Load images into ppm structure.
    //Hero image
    heroImage = ppm6GetImage("./images/hero.ppm");
    //Create texture elements
    glGenTextures(1, &heroTexture);
    int w = heroImage->width;
    int h = heroImage->height;
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //build a new set of data...
    unsigned char *silhouetteData = buildAlphaData(heroImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
    delete [] silhouetteData;

    gettimeofday(&fireStart, NULL);

    if (!QUICK_LOAD_TIME) {

        // load initialization screens
        unsigned char *menuData;
        glGenTextures(32, initTextures);
        string fileName;

        for (int q=0; q<32; q++) {

            fileName = "./images/init/init";
            fileName += itos(q);
            fileName += ".ppm";
            cout << "loading file: " <<fileName <<endl;
            initImages[q] = ppm6GetImage(fileName.c_str());

            glBindTexture(GL_TEXTURE_2D, initTextures[q]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            menuData = buildAlphaData(initImages[q]);
            w = initImages[q]->width;
            h = initImages[q]->height;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, menuData);
        }
        delete [] menuData;

        // load blinking computer screens
        unsigned char *computerData;
        glGenTextures(26, computerScreenTextures);
        fileName = "";

        for (int q=0; q<26; q++) {

            fileName = "./images/cs/cs";
            fileName += itos(q);
            fileName += ".ppm";
            cout << "loading file: " <<fileName <<endl;
            computerScreenImages[q] = ppm6GetImage(fileName.c_str());

            glBindTexture(GL_TEXTURE_2D, computerScreenTextures[q]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            computerData = buildAlphaData(computerScreenImages[q]);
            w = computerScreenImages[q]->width;
            h = computerScreenImages[q]->height;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, computerData);
        }
        delete [] computerData;
    }

    makePlatform(20, 1000, -16, 600);
    makePlatform(400, 16, 400, 70);
    makePlatform(200, 16, 900, 200);
    makePlatform(150, 16, 1200, 360);
    makePlatform(250, 16, 1450, 70);
    makePlatform(440, 16, 2500, 70);
    makePlatform(340, 16, 2300, 360);
    makePlatform(250, 16, 2800, 480);
    makePlatform(440, 16, 3500, 70);
    makePlatform(440, 16, 4000, 200);
    makePlatform(440, 16, 4500, 70);
    makePlatform(440, 16, 5500, 70);
    makePlatform(440, 16, 6500, 70);
    makePlatform(440, 16, 7500, 70);
    makePlatform(440, 16, 8500, 70);
    makePlatform(440, 16, 9500, 70);
    makePlatform(200, 16, 9700, 360);
    makePlatform(200, 16, 300, 200);
    makePlatform(20, 1000, -16, 600);

    //for (i=0;i<=100;i++){
    makeEnemy(37, 80, grounds[2], 1);
    //}
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[2], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[3], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);
    makeEnemy(37, 80, grounds[4], 1);


    // FIXME there are 40 image files, but currently only 1/3 of them work, the others
    // are all the same image
    /* 
       for (int q=0; q<12; q++) {
       string fileName;
       fileName = "./images/menuScreen";
       fileName += itos(q);
       fileName += ".ppm";
       cout << "loading file: " <<fileName <<endl;
       menuImage[q] = ppm6GetImage(fileName.c_str());

       glBindTexture(GL_TEXTURE_2D, menuTexture[q]);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       menuData = buildAlphaData(menuImage[q]);
       w = menuImage[q]->width;
       h = menuImage[q]->height;
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, menuData);
       }
       delete [] menuData;
       */  
}

void makePlatform(int w, int h, int x, int y) {
    grounds[grounds_length] = new Platform();
    grounds[grounds_length]->insert("./images/level.ppm", 1, 1);
    grounds[grounds_length]->init(w, h, x, y);
    grounds[grounds_length]->setupTile();
    grounds_length++;

}

void makeEnemy(int w, int h, Object *ground, int type) {
    if (enemies_length<MAX_ENEMIES){
        switch (type){
            case 1:
                enemies[enemies_length] = new Enemy(w, h, ground); 
                enemies[enemies_length]->insert("./images/enemy1.ppm", 26, 1);
                enemies[enemies_length]->setBottom(-44);
                enemies[enemies_length]->setLeft(-24);
                enemies[enemies_length]->setRight(24);
                enemies[enemies_length]->setTop(24);
                enemies[enemies_length]->setHeight(25);
                break;
        }
        enemies_length++;
    }
    else{
        cout << "Enemies array full!!!!" << endl;
    }
}

void check_mouse (XEvent *e) {
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
    if (e->xbutton.button==3) { //Right button was pressed
        return;
    }

    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x; //xpast = savex;
        savey = e->xbutton.y; //ypast = savey;
    }
}

int check_keys (XEvent *e) {
    //handle input from the keyboard
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape or key == XK_q) {
            return 1;
        }
        // Jump
        if ((key == XK_w || key == XK_Up) && !(hero->isDying)) {
            if (didJump < 2 && hero->getVelocityY() > -0.5) {
                didJump++;
                hero->setVelocityY(7);
            }
        }
        // move character left
        if ((key == XK_a || key == XK_Left) && !(hero->isDying)) {
            hero->setVelocityX(-6);
            lastFacing = 1;
        }
        // move character right
        if ((key == XK_d || key == XK_Right) && !(hero->isDying)) {
            hero->setVelocityX(6);
            lastFacing = 0;
        }
        // shooting
        if (key == XK_space) {
            hero->isShooting=1;
        }
        // debug death
        if (key == XK_q) {
            life-=1000;
        }
        // toggle start menu
        if (key == XK_m) {
            if (level) {
                level = 0;
            }
            else {
                level = 1;
            }
        }
        // cycle through screens for debugging
        if (key == XK_t) {
            frameIndex++;
        }
        //return 0;
    }
    else if (e->type == KeyRelease) {
        if ((key == XK_a || key == XK_Left) && !(hero->isDying)) {
            hero->setVelocityX(0);
            lastFacing = 1;
        }
        if ((key == XK_d || key == XK_Right) && !(hero->isDying)) {
            hero->setVelocityX(0);
            lastFacing = 0;
        }
        if (key == XK_space) {
            hero->isShooting=0;
        }
    }

    return 0;
}
int check_keys_menu (XEvent *e) {
    //handle input from the keyboard
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape or key == XK_q) {
            return 1;
        }
        // cycle through screens for debugging
        if (key == XK_t) {
            frameIndex++;
        }
        //return 0;
    }
    else if (e->type == KeyRelease) {
    }

    return 0;
}

bool detectCollide (Object *obj, Object *ground) {
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (obj->getRight() > ground->getLeft() &&
            obj->getLeft()   < ground->getRight() &&
            obj->getBottom() < ground->getTop()  &&
            obj->getTop()    > ground->getBottom());
}

bool bulletCollide (Bullet *b, Object *obj) {
    //Gets (Bullet, Object)
    //Reture True if Bullet Collides with Object
    return (obj->getRight() > b->pos[0] &&
            obj->getLeft()   < b->pos[0] &&
            obj->getBottom() < b->pos[1]  &&
            obj->getTop()    > b->pos[1]);
}

void groundCollide (Object *obj, Object *ground) {
    //(Moving Object, Static Object)
    //Detects Which boundaries the Moving Object is around the Static Object
    //top,down,left,right
    if (detectCollide(obj, ground)) {
        h_right=obj->getRight();
        h_left=obj->getLeft();
        h_top=obj->getTop();
        h_bottom=obj->getBottom();
        g_right=ground->getRight();
        g_bottom=ground->getBottom();
        g_top=ground->getTop();
        g_left=ground->getLeft();
        //If moving object is on top of the static object
        if (!(obj->getOldBottom() < g_top) && !(h_bottom >= g_top) && (obj->getVelocityY() < 0)) {
            obj->setVelocityY(0);
            obj->setCenter(obj->getCenterX(), g_top+(obj->getCenterY()-h_bottom));
            obj->setFloor(ground);
        }
        //If moving object is at the bottom of static object
        if (!(obj->getOldTop() > g_bottom) && !(h_top <= g_bottom)) {
            obj->setVelocityY(-0.51);
            obj->setCenter(obj->getCenterX(), g_bottom-(h_top-obj->getCenterY()));
        }
        //If moving object is at the l-eft side of static object
        if (!(obj->getOldRight() > g_left ) && !(h_right <= g_left)) {
            obj->setVelocityX(-0.51); obj->setCenter(g_left-(h_right-obj->getCenterX()), obj->getCenterY()
                    );
        }
        //If moving object is at the right side of static object
        if (!(obj->getOldLeft() < g_right ) && !(h_left >= g_right)) {
            obj->setVelocityX(0.51);
            obj->setCenter(g_right+(obj->getCenterX()-h_left), obj->getCenterY());
        }
    }
}

void movement() {
    // Hero Apply Velocity, Add Gravity
    hero->setOldCenter();
    hero->setCenter( (hero->getCenterX() + hero->getVelocityX()), (hero->getCenterY() + hero->getVelocityY()));
    hero->setVelocityY( hero->getVelocityY() - GRAVITY);
    //Detect Collisions
    for (i=0; i<grounds_length; i++) {
        groundCollide(hero, grounds[i]);
    }
    // Cycle through hero index sequences
    if (life<=0) {
        hero->setVelocityX(0);
        hero->isWalking=0;
        if (!(hero->isDying)) {
            hero->isDying=1;
            hero->setIndex(7);
            gettimeofday(&seqStart, NULL);
            fail=100;
        }
        else{
            gettimeofday(&seqEnd, NULL);
            if (((diff_ms(seqEnd, seqStart)) > 100) && (!hero->isFalling && !hero->isJumping)) {
                if ((hero->getIndex()<12)) {
                    hero->setIndex(hero->getIndex()+1);
                    gettimeofday(&seqStart, NULL);
                }
                else{
                    if (((diff_ms(seqEnd, seqStart)) > 500)) {
                        hero->setCenter(HERO_START_X, HERO_START_Y);
                        hero->isDying=0;
                        hero->setIndex(6);
                        life=100;
                        lives--;
                    }
                }
            }
        }
    }
    if ((hero->getVelocityY() < -1) && !(hero->isDying)) { // Falling
        hero->isFalling=1;
        hero->isJumping=0;
        hero->isWalking=0;
        if (didJump<1)
            hero->setIndex(7);
    } else if ((hero->getVelocityY() > 1) && !(hero->isDying)) { // Jumping
        hero->isJumping=1;
        hero->isWalking=0;
        hero->isFalling=0;
        if (didJump>1)
            hero->setIndex(0);
        else
            hero->setIndex(1);
    } else if (!hero->isDying) { // Walking
        if (hero->isFalling) { // Just hit ground object after fall
            hero->isFalling=0;
            hero->isJumping=0;
            didJump=0; // Reset jump counter
        }
        if (hero->getVelocityX() < -1 or
                hero->getVelocityX() > 1) {
            if (!(hero->isWalking) && !(hero->isJumping) && !(hero->isFalling)) { // Just started walking
                hero->isWalking=1;
                hero->setIndex(0); // Start walk sequence
                gettimeofday(&seqStart, NULL);
            }
            gettimeofday(&seqEnd, NULL);
            if (hero->isWalking && ((diff_ms(seqEnd, seqStart)) > 80)) { // Walk sequence
                hero->setIndex(((hero->getIndex()+1)%6));
                gettimeofday(&seqStart, NULL);
                hero->isFalling=0;
                hero->isJumping=0;
            }
        } else { // Standing idle
            hero->isWalking=0;
            if (!(hero->isJumping))
                hero->setIndex(6);
        }
    } else if (hero->isJumping && hero->getVelocityY() < -1) {
        hero->isJumping=0;
        hero->isFalling=1;
    } else if (hero->isFalling && hero->getVelocityY() > -1) {
        hero->isFalling=0;
    }
    if (!(hero->isWalking) and !(hero->isFalling) and !(hero->isJumping))
        hero->setVelocityX(0); // Prevent weird floating
    // Check for Death
    if (hero->getCenterY() < 0) {
        hero->setCenter(HERO_START_X, HERO_START_Y); // Respawn
        lives--;
        life=fail=100; // Reset life points, Display fail for 100 frames
        hero->setVelocityX(0);
        hero->isDying=0;
    }

    //Bullet creation
    Bullet *b;
    if (hero->isShooting){
        gettimeofday(&fireEnd, NULL);
        if (((diff_ms(fireEnd, fireStart)) > 250)) { //Fire rate 250ms
            gettimeofday(&fireStart, NULL); //Reset firing rate timer
            b = new Bullet;
            b->pos[0] = hero->getCenterX();
            b->pos[1] = hero->getCenterY()+15;
            if (lastFacing or hero->getVelocityX()<0) {
                b->vel[0] = -18;
            } else {
                b->vel[0] = 18;
            }
            b->vel[1] = 0;
            b->color[0] = 1.0f;
            b->color[1] = 1.0f;
            b->color[2] = 1.0f;
            b->next = bhead;
            if (bhead != NULL)
                bhead->prev = b;
            bhead = b;
            bullets++;
        }
    }

    //bullet collisions against grounds
    b = bhead;
    while (b) {
        //move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1];

        for (i=0;i<grounds_length;i++){
            if (bulletCollide(b,grounds[i]))
                deleteBullet(b);
        }
        //Check for collision with window edges
        if (b->pos[0] > WINDOW_WIDTH+roomX or b->pos[0] < 0) {
            deleteBullet(b);
        }
        b = b->next;
    }

    // Enemy movement, enemy ai
    for (i=0;i<enemies_length;i++) {
        enemies[i]->enemyAI(hero); //Where does enemy go?
        //enemyAI(enemies[i]);
        //bullets
        b = bhead;
        while (b) {
            if (bulletCollide(b,enemies[i])){
                deleteBullet(b);
                enemies[i]->life-=20;
                enemies[i]->setAggro(true);
            }
            b = b->next;
        }
        for (j=0; j<grounds_length; j++) {
            //Collision Detection
            groundCollide(enemies[i], grounds[j]); 
        }
        if ((enemies[i]->isDead) or enemies[i]->getCenterY()<0){
            deleteEnemy(i);
        }
    }
}

void renderInitMenu () {
    gettimeofday(&frameStart, NULL);
    int frameTime = 100;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) {
        frameIndex++;
        gettimeofday(&frameEnd, NULL);
    }

    if (frameIndex == 32) {
        cout << "frame index: "<< frameIndex <<endl;
        frameIndex = 0;
        level = 0;
        return;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, initTextures[frameIndex]);
    //glAlphaFunc(GL_GREATER, 0.0f);
    //glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);


}
void renderComputerScreenMenu () {
    gettimeofday(&frameStart, NULL);
    int frameTime = 300;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) {
        frameIndex++;
        gettimeofday(&frameEnd, NULL);
        //cout << "frame index: "<< frameIndex <<endl;
    }

    if (frameIndex == 26) {
        frameIndex = 0;
        level = 1;
        return;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, computerScreenTextures[frameIndex]);
    //glAlphaFunc(GL_GREATER, 0.0f);
    //glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);


}
void renderGrounds (int x, int y) {
    // render grounds
    for (i=0;i<grounds_length;i++) {
        if (inWindow(*(grounds[i]))) {
            //Platform
            glPushMatrix();
            glTranslatef(- x, - y, 0);
            grounds[i]->drawRow(0,0);
            glEnd(); glPopMatrix();
        }
    }
}

void renderEnemies (int x, int y) {
    for (int i=0;i<enemies_length;i++) {
        if (inWindow(*(enemies[i]))){
            glPushMatrix();
            glTranslatef(- x, - y, 0);
            enemies[i]->draw();
            glEnd(); glPopMatrix();
        }
    }
}

void renderHero (int x, int y) {
    // Draw Hero Sprite
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTranslatef( hero->getCenterX() - x, hero->getCenterY() - y, 0);
    int w = hero->getWidth();
    //int h = hero->getHeight();
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    float tl_sz = 0.076923077;
    // hero is facing left
    if ((hero->getVelocityX() < 0.0) or (hero->getOldCenterX()>hero->getCenterX())
            or lastFacing == 1) {
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero->getIndex()*tl_sz), 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero->getIndex()*tl_sz), 1.0f); glVertex2i(w,-w);
    }
    // hero is facing right
    else {
        glTexCoord2f(hero->getIndex()*tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f(hero->getIndex()*tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(w,-w);
    }
    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
}
void renderBullets (int x, int y) {
    //Draw the bullets
    Bullet *b = bhead;
    while (b) {
        //Log("draw bullet...\n");
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2f(b->pos[0]-x, b->pos[1]-y);
        glVertex2f(b->pos[0]-1.0f-x, b->pos[1]-y);
        glVertex2f(b->pos[0]+1.0f-x, b->pos[1]-y);
        glVertex2f(b->pos[0]-x, b->pos[1]-1.0f-y);
        glVertex2f(b->pos[0]-x, b->pos[1]+1.0f-y);
        glColor3f(0.8, 0.8, 0.8);
        glVertex2f(b->pos[0]-1.0f-x, b->pos[1]-1.0f-y);
        glVertex2f(b->pos[0]-1.0f-x, b->pos[1]+1.0f-y);
        glVertex2f(b->pos[0]+1.0f-x, b->pos[1]-1.0f-y);
        glVertex2f(b->pos[0]+1.0f-x, b->pos[1]+1.0f-y);
        glEnd();
        b = b->next;
    }
}

void render () {
    int x = roomX - WINDOW_HALF_WIDTH;
    int y = roomY - WINDOW_HALF_HEIGHT;
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw Background Falling Bits
    renderBackground();
    renderBullets(x, y);
    renderGrounds(x, y);
    renderEnemies(x, y);
    renderHero(x, y);

    //stringstream strs;
    //strs << i;
    //string temp_str = strs.str();
    //char* char_type = (char*) temp_str.c_str();

    if (fail>0) {
        writeWords("CRITICAL FAILURE", WINDOW_WIDTH/2- 200, WINDOW_HEIGHT/2);
        fail--;
    }
    writeWords("Lives:", WINDOW_WIDTH/2- 400, WINDOW_HEIGHT/2- 250);

}

void moveWindow () {
    double heroWinPosX = hero->getCenterX();
    double heroWinPosY = hero->getCenterY();

    //move window forward
    if ((heroWinPosX > roomX + interval) && ((roomX+WINDOW_HALF_WIDTH)<LEVEL_WIDTH-6)) {
        roomX+=6;
    }
    //move window backward (fast move if hero is far away)
    else if ((heroWinPosX < roomX - interval) && roomX>(WINDOW_HALF_WIDTH+6)) {
        roomX-=6;
        if (heroWinPosX < (roomX - interval - 400)) {
            roomX-=20;
        }
        if (heroWinPosX < (roomX - interval - 800)) {
            roomX-=50;
        }
    }
    //move window up
    if ((heroWinPosY > roomY + interval) && ((roomY+6)<(MAX_HEIGHT-WINDOW_HALF_HEIGHT))) {
        roomY+=6;
    }
    //move window down
    else if ((heroWinPosY < roomY - interval) && (roomY-6)>(WINDOW_HALF_HEIGHT)) {
        i = hero->getVelocityY();
        if (i>-6)
            i=-6;
        roomY+=i;
    }
}
void renderBackground () {
    if (bg < MAX_BACKGROUND_BITS) {
        // Create bit
        bgBit *bit = new bgBit;
        if (bit == NULL) {
            exit(EXIT_FAILURE);
        }
        bit->pos[0] = (rnd() * ((float)LEVEL_WIDTH + (roomX-(WINDOW_WIDTH/2)) - 1000));
        bit->pos[1] = rnd() * 100.0f + (float)WINDOW_HEIGHT +
            (roomY-(WINDOW_HEIGHT/2));
        bit->pos[2] = 0.8 + (rnd() * 0.4);
        bit->vel[0] = 0.0f;
        bit->vel[1] = -0.8f;
        bit->vel[2] = (rnd());
        bit->next = bitHead;
        if (bitHead != NULL)
            bitHead->prev = bit;
        bitHead = bit;
        bg++;
    }
    // Reset pointer to beginning to render all bits
    bgBit *bit = bitHead;
    while (bit) {
        VecCopy(bit->pos, bit->lastpos);
        if (bit->pos[1] > 0) {
            bit->pos[1] += bit->vel[1];

        }
        else{
            bgBit *savebit = bit->next;
            if (bit->prev == NULL) {
                if (bit->next == NULL) {
                    bitHead = NULL;
                } else {
                    bit->next->prev = NULL;
                    bitHead = bit->next;
                }
            } else {
                if (bit->next == NULL) {
                    bit->prev->next = NULL;
                } else {
                    bit->prev->next = bit->next;
                    bit->next->prev = bit->prev;
                }
            }
            delete bit;
            bit = savebit;
            bg--;
            continue;
        }
        int center = (bit->pos[0]-((roomX-WINDOW_HALF_WIDTH)*bit->pos[2]));
        int j = bit->pos[2];
        if (bit->pos[1]>(WINDOW_HEIGHT*0.7)) {
            i=255;
        } else {
            i = (bit->pos[1]>(WINDOW_HEIGHT*0.7));
            if (i<1)
                i=0;
        }

        if (j>=1) {
            writePixel(1, center, bit->pos[1]);
        } else if (j>0.9) {
            writePixel(0, center, bit->pos[1]);
        } else {
            writePixel(0, center, bit->pos[1]);
        }
        bit = bit->next;
    }
    glLineWidth(1);
}

void deleteBullet(Bullet *node) {
    //remove a node from linked list
    if (node->prev == NULL) {
        if (node->next == NULL) {
            bhead = NULL;
        } else {
            node->next->prev = NULL;
            bhead = node->next;
        }
    } else {
        if (node->next == NULL) {
            node->prev->next = NULL;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
    }
    delete node;
    node = NULL;
    bullets--;
}

void deleteEnemy(int ind){
    enemies_length--;
    delete enemies[i];
    enemies[i] = enemies[enemies_length];
    enemies[enemies_length]=NULL;

/*
    for (i=ind;i<enemies_length;i++){
        enemies[i]=(i==(MAX_ENEMIES-1)?NULL:enemies[i+1]);
        cout << "e[" << i << "] = " << (i==(MAX_ENEMIES-1)?NULL:enemies[i+1]);
    } 
    enemies_length--;
    cout << "deleted enemy, count : " << enemies_length << endl;
    */
}

void cleanup_background(void) {
    bgBit *s;
    while (bitHead) {
        s = bitHead->next;
        delete bitHead;
        bitHead = s;
    }
    bitHead = NULL;
}

Object createAI (int w, int h, Object *ground) {
    Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
    return newEnemy;

}

