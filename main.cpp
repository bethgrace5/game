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
#include <sstream>
#include <algorithm>
#include "fastFont.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define WINDOW_HALF_WIDTH  WINDOW_WIDTH/2
#define WINDOW_HALF_HEIGHT WINDOW_HEIGHT/2
#define LEVEL_WIDTH 10000
#define MAX_HEIGHT 1200
#define GRAVITY 0.2
#define MAX_GROUNDS 32
#define rnd()(float)rand() /(float)(RAND_MAX)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]

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

int diff_ms (timeval t1, timeval t2) {
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}

template <typename T>
string itos (T Number) {
    stringstream ss;
    ss << Number;
    return (ss.str());
}

//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Hero Globals
int didJump=0;
int lives=3;
int life=100;
int isWalking=0;
int isFalling=0;
int isDying=0;
int isJumping=0;
// last facing 0 means facing forward(right), 1 means backward(left)
int lastFacing=0;
double h_right, h_left, h_top, h_bottom;
timeval seqStart, seqEnd;

//Game Globals
string str = "";
bgBit *bitHead = NULL;
Object *grounds[MAX_GROUNDS] = {NULL};
Object *enemies[32] = {NULL};
int bg, grounds_length, enemies_length,  i, j, level=0;
int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;
int fail=0;
int interval=120;
double g_left, g_right, g_top, g_bottom;
int quit=0;

//Images and Textures
Ppmimage *heroImage=NULL;
GLuint heroTexture;

Ppmimage *menuImage[4];
GLuint menuTexture[4];

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e);
int  check_keys (XEvent *e, Object *hero);
void movement(Object *hero);
void render(Object *hero);
void moveWindow(Object *hero);
void renderBackground(void);
void cleanup_background(void);
void renderGrounds(int x, int y);
void renderEnemies(int x, int y);
void renderHero(Object *hero, int x, int y);
Object createAI( int w, int h, Object *ground);

void groundCollide(Object *obj, Object *ground);
bool detectCollide(Object *obj, Object *ground);

bool inWindow(Object &obj) {
    return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
                obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or
            (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
             obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
}
void renderMenu () {

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glBindTexture(GL_TEXTURE_2D, menuTexture[0]);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    //glAlphaFunc(GL_GREATER, 0.0f);
    //glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    //float w = menuImage[0]->width;
    //float h = menuImage[0]->height;

    glTexCoord2f(0.1, 0.9) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0.1, 0.1) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(0.9, 0.1) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(0.9, 0.9) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);

}

int main(void) {
    initXWindows(); init_opengl();

    //declare hero object
    Object hero(46, 48, HERO_START_X, HERO_START_Y);
    hero.setTop(44);
    hero.setBottom(-44);
    hero.setLeft(-26);
    hero.setRight(26);

    Object ground_0(10, 1000, -10, 600);
    Object ground_1(400, 10, 400, 80);
    Object ground_2(200, 10, 900, 200);
    Object ground_3(150, 10, 1200, 360);
    Object ground_4(250, 10, 1450, 80);
    Object ground_5(440, 10, 2500, 80);
    Object ground_6(340, 10, 2300, 360);
    Object ground_7(250, 10, 2800, 480);
    Object ground_8(440, 10, 3500, 80);
    Object ground_9(440, 10, 4000, 200);
    Object ground_10(440, 10, 4500, 80);
    Object ground_11(440, 10, 5500, 80);
    Object ground_12(440, 10, 6500, 80);
    Object ground_13(440, 10, 7500, 80);
    Object ground_14(440, 10, 8500, 80);
    Object ground_15(440, 10, 9500, 80);
    Object ground_16(200, 10, 9700, 360);
    Object ground_17(200, 10, 300, 180);

    grounds[0] = &ground_0;
    grounds[1] = &ground_1;
    grounds[2] = &ground_2;
    grounds[3] = &ground_3;
    grounds[4] = &ground_4;
    grounds[5] = &ground_5;
    grounds[6] = &ground_6;
    grounds[7] = &ground_7;
    grounds[8] = &ground_8;
    grounds[9] = &ground_9;
    grounds[10] = &ground_10;
    grounds[11] = &ground_11;
    grounds[12] = &ground_12;
    grounds[13] = &ground_13;
    grounds[14] = &ground_14;
    grounds[15] = &ground_15;
    grounds[16] = &ground_16;
    grounds[17] = &ground_17;
    grounds_length=18;

    //setup enemies
    Object enemy_0 = createAI(20, 48, &ground_2);
    Object enemy_1 = createAI(20, 48, &ground_3);

    enemies[0] = &enemy_0;
    enemies[1] = &enemy_1;
    enemies_length=1;
    level = 1;

    while (!quit) { //Staring Animation
        while (XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e);
            quit = check_keys(&e, &hero);
        }
        if (level == 0) {
            renderMenu();
        }
        else {
            movement(&hero);
            render(&hero);
            moveWindow(&hero);
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

unsigned char *buildAlphaData (Ppmimage *img) {
    //add 4th component to RGB stream...
    int a,b,c;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    //newdata = (unsigned char *)malloc(img->width * img->height * 4);
    newdata = new unsigned char[img->width * img->height * 4];
    ptr = newdata;
    for (i=0; i<img->width * img->height * 3; i+=3) {
        a = *(data+0);
        b = *(data+1);
        c = *(data+2);
        *(ptr+0) = a;
        *(ptr+1) = b;
        *(ptr+2) = c;
        //get the alpha value
        *(ptr+3) = (a|b|c);
        ptr += 4;
        data += 3;
    }
    return newdata;
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

    menuImage[0] = ppm6GetImage("./images/menuScreen0.ppm");
    //menuImage[1] = ppm6GetImage("./images/menuScreen1.ppm");
    //menuImage[2] = ppm6GetImage("./images/menuScreen2.ppm");
    //menuImage[3] = ppm6GetImage("./images/menuScreen3.ppm");

    unsigned char *menuData;
    //for (i=0; i<1; i++) {
        glGenTextures(2, &menuTexture[0]);
        glBindTexture(GL_TEXTURE_2D, menuTexture[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        menuData = buildAlphaData(menuImage[0]);
        w = menuImage[0]->width;
        h = menuImage[0]->height;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, menuData);
    //}

    delete [] menuData;
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

int check_keys (XEvent *e, Object *hero) {
    //handle input from the keyboard
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape or key == XK_q) {
            return 1;
        }
        // Jump
        if ((key == XK_w || key == XK_Up) && !isDying) {
            if (didJump < 2 && hero->getVelocityY() > -0.5) {
                didJump++;
                hero->setVelocityY(7);
            }
        }
        // move character left
        if ((key == XK_a || key == XK_Left) && !isDying) {
            hero->setVelocityX(-6);
            lastFacing = 1;
        }
        // move character right
        if ((key == XK_d || key == XK_Right) && !isDying) {
            hero->setVelocityX(6);
            lastFacing = 0;
        }
        // debug death
        if (key == XK_space) {
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

        //return 0;
    }
    else if (e->type == KeyRelease) {
        if ((key == XK_a || key == XK_Left) && !isDying) {
            hero->setVelocityX(0);
            lastFacing = 1;
        }
        if ((key == XK_d || key == XK_Right) && !isDying) {
            hero->setVelocityX(0);
            lastFacing = 0;
        }
        if (key == XK_space) {
            life-=1000;
        }
    }

    return 0;
}

void enemyAI (Object *hero, Object *enemy) {
    Object *e_f = enemy->getFloor();
    Object *h_f = hero->getFloor();
    string old = str;
    str = "";
    if (!h_f) {
        return;
    }
    str += "enemy " + itos(enemy) + " ";
    float h_cx=hero->getCenterX();
    //float h_cy=hero->getCenterY();
    //float h_vx=hero->getVelocityX();
    float h_fl=h_f->getLeft();
    float h_ft=h_f->getTop();
    float h_fr=h_f->getRight();
    //---------------------------------
    float e_cx=enemy->getCenterX();
    float e_cy=enemy->getCenterY();
    float e_vx=enemy->getVelocityX();
    float e_vy=enemy->getVelocityY();

    if (e_f && h_f) { // If enemy is not mid jump
        float e_fl=e_f->getLeft();
        float e_ft=e_f->getTop();
        float e_fr=e_f->getRight();
        float d_x=e_cx - hero->getCenterX();
        float d_y=e_cy - hero->getCenterY();
        int h_above = (d_y<0)?10:0;
        int h_right = (d_x<0)?1:0;
        int range=(enemy->getAggro())?800:400;
        int h_close = (((d_x*d_x)+(d_y*d_y)<(range*range) && !isDying)?(((d_x*d_x)+(d_y*d_y)<(200*200))?2:1):0);
        int h_dir = h_above+h_right;
        if (!isJumping and !isFalling)
            h_dir+=((h_f==e_f)?100:0);
        str += "[DIR: " + itos(h_dir) + "]";
        // If enemy is within 200px & not dead: 2; within 400px: 1; else: 0
        switch (h_close) {
            case 2: // hero close range
                enemy->setVelocityX(0);
                str += "attack!";
                //fall through?
                break;
            case 1: // follow hero
                if (!enemy->getAggro())
                    enemy->setAggro(true);
                str += "Aggro! ";
                switch (h_dir) {
                    case 0: // If hero is to the lower left of enemy
                        if (e_fl<e_cx) { // If enemy won't fall
                            enemy->setVelocityX(-6); // then move to the left
                            str += "move left!";
                        } else { // If enemy is going to fall
                            enemy->setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fr>e_fl) {//grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<e_ft) {
                                    enemy->setVelocityX(-6);// Jump down
                                    str += "jump down!";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(1)";
                                }
                            }
                            else{ // gap
                                if ((e_fl-h_fr)<(e_ft-h_ft)) { // just run off
                                    enemy->setVelocityX(-6);
                                    str += "move left!";
                                    enemy->setFloor(NULL);
                                }
                                else if (h_fr>(e_fl-640)) {
                                    if ((h_cx+250)<e_cx) {
                                        enemy->setVelocityX(-6);// Jump over
                                    }
                                    else{
                                        enemy->setVelocityX(((h_fr-e_fl)/59)*-1);// Jump over
                                        if (enemy->getVelocityX()<-6)
                                            enemy->setVelocityX(-6);
                                    }
                                    enemy->setVelocityY(6);
                                    str += "jump over!";
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 1: // If hero is to the lower right of enemy
                        if (e_fr>e_cx) { // If enemy won't fall
                            enemy->setVelocityX(6); // then move to the right
                            str += "move right!";
                        } else { // If enemy is going to fall if he keeps going
                            enemy->setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fl<=e_fr) { // grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<=e_ft) {
                                    enemy->setVelocityX(6);// Jump down
                                    str += "jump down!";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(2)";
                                }
                            }
                            else{ // gap
                                if ((h_fl-e_fr)<(e_ft-h_ft)) { // just run off
                                    enemy->setVelocityX(6);
                                    str += "move right!";
                                    enemy->setFloor(NULL);
                                }
                                else if (h_fl<(e_fr+640)) {
                                    if ((h_cx-250)>e_cx) {
                                        enemy->setVelocityX(6);// Jump over
                                    }
                                    else{
                                        enemy->setVelocityX(((e_fr-h_fl)/59)*-1);// Jump over
                                        if (enemy->getVelocityX()>6)
                                            enemy->setVelocityX(6);
                                    }
                                    enemy->setVelocityY(6);
                                    str += "jump over!";
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 10: // If hero is to the upper left of enemy
                        //hero's ground is above enemy's ground
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fr>e_fl) { // grounds are overlapping
                                if ((enemy->getLeft()-6)<h_fr) {
                                    enemy->setVelocityX(6);
                                    str += "move right";
                                }
                                else if ((enemy->getLeft()-14)>h_fr) {
                                    enemy->setVelocityX(-6);
                                    str += "move left";
                                }
                                else{
                                    enemy->setVelocityX(0);
                                    enemy->setVelocityY(7);
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the left with a gap
                                if ((enemy->getLeft())>e_fl) {
                                    enemy->setVelocityX(-6);
                                    str += "move left";
                                }
                                else{
                                    enemy->setVelocityY(7);
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the left
                            if ((enemy->getLeft())>e_fl) {
                                enemy->setVelocityX(-6);//move to edge
                                str += "move left";
                            }
                            else if (h_f!=e_f) { // gap
                                enemy->setVelocityX(0);
                                if ((e_fl-h_fr)<(e_ft-h_ft)) { // just run off
                                    enemy->setVelocityX(-6);
                                    str += "move left!";
                                    enemy->setFloor(NULL);
                                }
                                else if (h_fr>(e_fl-640)) {
                                    if ((h_cx+250)<e_cx) {
                                        enemy->setVelocityX(-6);// Jump over
                                    }
                                    else{
                                        enemy->setVelocityX(((h_fr-e_fl)/59)*-1);// Jump over
                                        if (enemy->getVelocityX()<-6)
                                            enemy->setVelocityX(-6);
                                    }
                                    enemy->setVelocityY(6);
                                    str += "jump over!";
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 11: // If hero is to the upper right of enemy
                        //hero's ground is above enemy's ground
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fl<e_fr) {
                                if ((enemy->getRight()+6)>h_fl) {
                                    enemy->setVelocityX(-6);
                                    str += "move left";
                                }
                                else if ((enemy->getRight()+14)<h_fl) {
                                    enemy->setVelocityX(6);
                                    str += "move right";
                                }
                                else{
                                    enemy->setVelocityX(0);
                                    str += "stop, ";
                                    enemy->setVelocityY(7);
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the right with a gap
                                if ((enemy->getRight())<e_fr) {
                                    enemy->setVelocityX(6);
                                    str += "move left";
                                }
                                else{
                                    enemy->setVelocityY(7);
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the right
                            if ((enemy->getRight())<e_fr) {
                                enemy->setVelocityX(6);//move to edge
                                str += "move right";
                            }
                            else if (h_f!=e_f) { // gap
                                enemy->setVelocityX(0);
                                if ((h_fl-e_fr)<(e_ft-h_ft)) { // just run off
                                    enemy->setVelocityX(6);
                                    str += "move right!";
                                    enemy->setFloor(NULL);
                                }
                                else if (h_fl<(e_fr+640)) {
                                    if ((h_cx-250)>e_cx) {
                                        enemy->setVelocityX(6);// Jump over
                                    }
                                    else{
                                        enemy->setVelocityX(((e_fr-h_fl)/59)*-1);// Jump over
                                        if (enemy->getVelocityX()>6)
                                            enemy->setVelocityX(6);
                                    }
                                    enemy->setVelocityY(6);
                                    str += "jump over!";
                                    enemy->setJump();
                                    str += "jump #" + itos(enemy->getJump()) + " ";
                                    enemy->setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 100:
                    case 110:
                        enemy->setVelocityX(-6);
                        str += "move left!";
                        break;
                    case 101:
                    case 111:
                        enemy->setVelocityX(6);
                        str += "move right!";
                        break;
                }
                break;
            default: // patrol
                enemy->setAggro(false);
                if (e_vx==0) {
                    enemy->setVelocityX((rnd()>.5)?(-0.6):(0.6));//Patrol ground object
                    str += "start patrolling";
                } else {
                    if (e_vx<0 && e_f!=NULL) {
                        if (enemy->getLeft()<e_fl) {
                            e_vx*=-1; //must set this value for following code
                            enemy->setVelocityX(e_vx);
                            str += "turn around";
                        }
                    } else if (e_vx>0 && e_f!=NULL) {
                        if (enemy->getRight()>e_fr) {
                            e_vx*=-1; //must set this value for following code
                            enemy->setVelocityX(e_vx);
                            str += "turn around";
                        }
                    }
                }
                break;
        }
        //Check for falling off
        if (enemy->getFloor()) { // don't check e_f here
            if (e_vx>0 && e_cx > e_fr) {
                enemy->setVelocityX(0);
                str += "almost fell!";
            }
            if (e_vx<0 && e_cx < e_fl) {
                enemy->setVelocityX(0);
                str += "almost fell!";
            }
        }
    }
    else if (h_f) { // enemy is mid jump
        if (e_vx==0) {
            if ((e_vy > -0.5) &&
                    (e_vy <= 0)) {
                if (enemy->getBottom() < h_ft) { // enemy needs to double jump
                    if (enemy->getJump()<2) {
                        enemy->setVelocityY(7);
                        enemy->setJump();
                        str += "jump #" + itos(enemy->getJump()) + " ";
                    }
                }
                else if (enemy->getBottom() > h_ft) {
                    if (e_cx<h_cx) {
                        if (e_cx>(h_cx-100)) {
                            enemy->setVelocityX(1);
                        }
                        else if (e_cx>(h_cx-250)) {
                            enemy->setVelocityX(3);// Jump over
                        }
                        else{
                            enemy->setVelocityX(6);
                        }
                    }
                    else{
                        if (e_cx<(h_cx+100)) {
                            enemy->setVelocityX(-1);
                        }
                        else if (e_cx<(h_cx+250)) {
                            enemy->setVelocityX(-3);// Jump over
                        }
                        else{
                            enemy->setVelocityX(-6);
                        }
                    }
                    str += "move";
                }
            }
        }
        else{ //enemy is moving left or right
            e_vx=enemy->getVelocityX();
            e_vy=enemy->getVelocityY();
            if (e_cx>h_cx && e_vx>0) {
                enemy->setVelocityX(0);
            }
            else if (e_cx<h_cx && e_vx<0) {
                enemy->setVelocityX(0);
            }
            if ((e_vy > -0.5) && (e_vy <= 0)) { // check if enemy needs to double jump
                if (((e_vx > 0) && (e_cy < (h_ft+360)) && (e_cx < (h_fl-180))) or
                        ((e_vx < 0) && (e_cy > (h_ft+360)) && (e_cx > (h_fr+180)))) {
                    // enemy double jump to lower platform
                    if (enemy->getJump()<2) {
                        enemy->setVelocityY(7);
                        enemy->setJump();
                        str += "jump #" + itos(enemy->getJump()) + " ";
                    }
                }
                else{
                    str += "too far? my x,y:" + itos(e_cx) + "," + itos(e_cy) + ";" + "ground's x,y:";
                    if (e_vx>0)
                        str += itos(h_fl) + "," + itos(h_ft);
                    else
                        str += itos(h_fr) + "," + itos(h_ft);
                }
            }
        }
    }
    if ((str.length())>24 && str != old) {
        //cout << str << endl;
    }
}

bool detectCollide (Object *obj, Object *ground) {
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (obj->getRight()  > ground->getLeft() &&
            obj->getLeft()   < ground->getRight() &&
            obj->getBottom() < ground->getTop()  &&
            obj->getTop()    > ground->getBottom());
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

void movement(Object *hero) {
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
        isWalking=0;
        if (!isDying) {
            isDying=1;
            hero->setIndex(7);
            gettimeofday(&seqStart, NULL);
            fail=100;
        }
        else{
            gettimeofday(&seqEnd, NULL);
            if (((diff_ms(seqEnd, seqStart)) > 100) && (!isFalling && !isJumping)) {
                if ((hero->getIndex()<12)) {
                    hero->setIndex(hero->getIndex()+1);
                    gettimeofday(&seqStart, NULL);
                }
                else{
                    if (((diff_ms(seqEnd, seqStart)) > 500)) {
                        hero->setCenter(HERO_START_X, HERO_START_Y);
                        isDying=0;
                        hero->setIndex(6);
                        life=100;
                        lives--;
                    }
                }
            }
        }
    }
    if ((hero->getVelocityY() < -1) && !isDying) { // Falling
        isFalling=1;
        isJumping=isWalking=0;
        if (didJump<1)
            hero->setIndex(7);
    } else if ((hero->getVelocityY() > 1) && !isDying) { // Jumping
        isJumping=1;
        isWalking=isFalling=0;
        if (didJump>1)
            hero->setIndex(0);
        else
            hero->setIndex(1);
    } else if (!isDying) { // Walking
        if (isFalling) { // Just hit ground object after fall
            isFalling=isJumping=didJump=0; // Reset jump counter
        }
        if (hero->getVelocityX() < -1 or
                hero->getVelocityX() > 1) {
            if (!isWalking && !isJumping && !isFalling) { // Just started walking
                isWalking=1;
                hero->setIndex(0); // Start walk sequence
                gettimeofday(&seqStart, NULL);
            }
            gettimeofday(&seqEnd, NULL);
            if (isWalking && ((diff_ms(seqEnd, seqStart)) > 80)) { // Walk sequence
                hero->setIndex(((hero->getIndex()+1)%6));
                gettimeofday(&seqStart, NULL);
                isFalling=isJumping=0;
            }
        } else { // Standing idle
            isWalking=0;
            if (!isJumping)
                hero->setIndex(6);
        }
    } else if (isJumping && hero->getVelocityY() < -1) {
        isJumping=0;
        isFalling=1;
    } else if (isFalling && hero->getVelocityY() > -1) {
        isFalling=0;
    }
    if (!isWalking and !isFalling and !isJumping)
        hero->setVelocityX(0); // Prevent weird floating
    // Check for Death
    if (hero->getCenterY() < 0) {
        hero->setCenter(HERO_START_X, HERO_START_Y); // Respawn
        lives--;
        life=fail=100; // Reset life points, Display fail for 100 frames
        hero->setVelocityX(0);
        isDying=0;
    }
    // Enemy movement, enemy ai
    for (i=0;i<enemies_length;i++) {
        enemyAI(hero, enemies[i]); //Where does enemy go?
        enemies[i]->setOldCenter();
        enemies[i]->setCenter( //Apply Physics
                (enemies[i]->getCenterX() + enemies[i]->getVelocityX()),
                (enemies[i]->getCenterY() + enemies[i]->getVelocityY()));
        enemies[i]->setVelocityY( enemies[i]->getVelocityY() - GRAVITY);
        for (j=0; j<grounds_length; j++) {
            groundCollide(enemies[i], grounds[j]); //Collision Detection
        }
    }
}

void renderGrounds (int x, int y) {
    int w, h;
    glColor3ub(65,155,225);
    // render grounds
    Object *ground;
    for (i=0;i<grounds_length;i++) {
        ground = grounds[i];
        if (inWindow(*ground)) {
            //Ground
            glPushMatrix();
            glTranslatef( ground->getCenterX() - x, ground->getCenterY() - y, 0);
            w = ground->getWidth();
            h = ground->getHeight();
            glBegin(GL_QUADS);
            glVertex2i(-w,-h);
            glVertex2i(-w, h);
            glVertex2i( w, h);
            glVertex2i( w,-h);
            glEnd(); glPopMatrix();

        }
    }
}
void renderEnemies (int x, int y) {
    int w, h;
    // render enemies
    glColor3ub(100,0,0);
    Object *enemy;
    for (int i=0;i<enemies_length;i++) {
        enemy = enemies[i];
        if (inWindow(*enemy)) {
            //Enemy
            glPushMatrix();
            glTranslatef( enemy->getCenterX() - x, enemy->getCenterY() - y, 0);
            w = enemy->getWidth();
            h = enemy->getHeight();
            glBegin(GL_QUADS);
            glVertex2i(-w,-h);
            glVertex2i(-w, h);
            glVertex2i( w, h);
            glVertex2i( w,-h);
            glEnd(); glPopMatrix();
        }
    }
}

void renderHero (Object *hero, int x, int y) {
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

void render (Object *hero) {
    int x = roomX - WINDOW_HALF_WIDTH;
    int y = roomY - WINDOW_HALF_HEIGHT;
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw Background Falling Bits
    renderBackground();

    renderGrounds(x, y);
    renderEnemies(x, y);
    renderHero(hero, x, y);

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

void moveWindow (Object *hero) {
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

