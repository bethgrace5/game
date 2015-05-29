#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <GL/glx.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "Enemy.h"
#include "Item.h"
#include "Player.h"
#include "Platform.h"
#include "definitions.h"
#include "functions.h"
#include "Object.h"
#include "sounds.cpp"
#include "Storage.cpp"
#include "fastFont.cpp"
//#include "Animate.h"

using namespace std;
//typedef double Vec[3];

// background bits
struct bgBit {
    Vec pos;
    Vec lastpos;
    Vec vel;
    struct bgBit *next;
    struct bgBit *prev;
    const char* n;
};

// time difference in milliseconds
Sprite bulletImage;
//Animate explode;
int animateOn = 0;
/*struct Bullet {
    int type; //0 = boss, 1 = enemy 2, 2 = hero, 3 = enemy 1
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
*/
//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Hero Globals
int didJump=0, life=100, lastFacing=0;
double h_right, h_left, h_top, h_bottom;
timeval seqStart, seqEnd; // hero's sprite index
timeval fireStart, fireEnd; // hero's fire rate timer
float healthIndex = 0;

//Game Globals
bgBit *bitHead = NULL;
Bullet *bulletHead = NULL;
Enemy *enemies[MAX_ENEMIES];
Player *hero;
Platform *grounds[MAX_GROUNDS];
Item *items;
Item *itemsHold[10];
int items_length = 0;
double g_left, g_right, g_top, g_bottom;
int bg, bullets, grounds_length, enemies_length, i, j, level=0, fail=0, quit=0;
int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;

// menu rendering and selection Globals
int showInvalid=0, frameIndex=0, menuSelection = 0;
timeval frameStart, frameEnd;

//Images and Textures
Ppmimage *initImages[32], *computerScreenImages[32], *healthBarImage[1], *lifeImage[1];
GLuint initTextures[65], computerScreenTextures[32], healthBarTexture[1], lifeTexture[1];

//Function prototypes
Object createAI( int w, int h, Object *ground);
bool bulletCollide(Bullet *b, Object *obj);
bool detectCollide(Object *obj, Object *ground);
bool detectItem (Object *obj, Item *targetItem);
int  check_keys (XEvent *e);
void check_mouse(XEvent *e);
void cleanupXWindows(void);
void cleanup_background(void);
void deleteBullet(Bullet *node);
void deleteEnemy(int ind);
void groundCollide(Object *obj, Object *ground);
void initXWindows(void);
void init_opengl(void);
void makeEnemy(int w, int h, Object *ground, int type); 
void makePlatform(int w, int h, int x, int y);
void makeItems(int w, int h, int x, int y);
void moveWindow(void);
void movement(void);
void render(void);
void renderBackground(void);
void renderBullets(int x, int y);
void renderComputerScreenMenu();
void renderEnemies(int x, int y);
void renderGrounds(int x, int y);
void renderHero(int x, int y);
void renderAnimations(int x, int y);
void renderItems(int x, int y);
void renderInitMenu();
void renderHealthBar();
void renderLives();



int main(void) {
    initXWindows(); init_opengl(); 
#ifdef USE_SOUND
    init_sounds();
    fmod_playsound(scaryAmbience);
#endif

    //declare hero object

    hero = new Player();
    hero->insert("./images/hero.ppm", 13, 1);
    hero->setSize(44,48);

    bulletImage.insert("./images/bullets.ppm",4, 1);
    bulletImage.setSize(22, 10);

    //explode.insert("./images/hero.ppm", 4, 2);
    //explode.setSize(400,400);
    // skip menu and go straight to level 1

    // Item test
    items = new Item();
    items->insert("./images/level2.ppm", 1, 1);
    items->setSize(20,20);
    //end test

    if(QUICK_LOAD_TIME) {
        level = 1;
    }

    while (!quit) { //Staring Animation
        while (XPending(dpy)) {
            XEvent e; XNextEvent(dpy, &e);
            quit = check_keys(&e);
        }
        if (level == -1) {
            renderInitMenu();
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

#ifdef USE_SOUND
    fmod_cleanup();
#endif
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
    //Create texture elements
    int w;
    int h;

    gettimeofday(&fireStart, NULL);
    gettimeofday(&frameStart, NULL);

    string fileName;

    if (!QUICK_LOAD_TIME) {
        // load initialization screens
        unsigned char *menuData;
        glGenTextures(65, initTextures);

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

        // repeat blinking dot for several frames
        // frame 30 has no dot, frame 31 has a dot
        initTextures[32] = initTextures[30];
        initTextures[33] = initTextures[30];

        // repeat dot blinking frames in initialization sequence
        // by repeating frames that have already been loaded
        // after 4 iterations, add 5
        int offset = 1;
        for (int i=34; i<65; i++) {
            initTextures[i] = initTextures[30];
            initTextures[i+4] = initTextures[31];
            if( offset == 4 ){
                i+=4;
                offset = 0;
            }
            offset++;
        }
        initTextures[31] = initTextures[30];


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

    // load life image
    unsigned char *lifeData;
    glGenTextures(1, lifeTexture);
    lifeImage[0] = ppm6GetImage("./images/life.ppm");
    glBindTexture(GL_TEXTURE_2D, lifeTexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    lifeData = buildAlphaData(lifeImage[0]);
    w = lifeImage[0]->width;
    h = lifeImage[0]->height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, lifeData);
    delete [] lifeData;

    // load health bar image
    glGenTextures(1, healthBarTexture);
    healthBarImage[0] = ppm6GetImage("./images/healthBar.ppm");
    glBindTexture(GL_TEXTURE_2D, healthBarTexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    unsigned char *healthData = buildAlphaData(healthBarImage[0]);
    w = healthBarImage[0]->width;
    h = healthBarImage[0]->height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, healthData);
    delete [] healthData;


    if( USE_TOOLS ){
        // loads platform objects from file
        ifstream dfs("test.ros", ios::binary);
        dfs.read((char *)&storeIn, sizeof(storeIn));

        // pulls in created objects from store
        cout << "what is the length in storeIn " << storeIn.grounds_length << "\n";
        for(int i = 0; i < storeIn.grounds_length; i++){
            grounds[i] = &storeIn.grounds[i];
            grounds[i]->reInitSprite();
            grounds_length++;
        }
    }
    else {
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

        makeItems(16, 20, 375, 232);
        makeItems(16, 20, 975, 232);

    }

    makeEnemy(37, 80, grounds[2], 1);

    makeEnemy(43, 42, grounds[1], 2);
    makeEnemy(43, 42, grounds[2], 2);
    makeEnemy(43, 42, grounds[2], 2);
    makeEnemy(43, 42, grounds[2], 2);
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

}

void check_mouse (XEvent *e) {
    static int savex = 0, savey = 0;
    if (e->type == ButtonRelease) { return;}
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) { //Left button was pressed
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

int check_keys (XEvent *e) {
    //handle input from the keyboard
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (level==1) {
            if (key == XK_Escape or key == XK_q) {
                return 1;
            }
            // Jump
            if ((key == XK_w || key == XK_Up)) {
                hero->jump();
            }
            // move character left
            if (key == XK_a || key == XK_Left) {
                hero->moveLeft();
            }
            // move character right
            if (key == XK_d || key == XK_Right){
                hero->moveRight();
            }
            // shooting
            if (key == XK_space) {
                hero->setShooting(true);
            }
            // debug death
            if (key == XK_y) {
                // cycleAnimations() checks for 0 or less health
                // to show dying sequence
                hero->decrementLives();
#ifdef USE_SOUND
                fmod_playsound(dunDunDun);
#endif
            }
            if (key == XK_h) {
                healthIndex--;
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
            // play sounds for debugging
            if (key == XK_t) {
            }
            if (key == XK_u){
                animateOn = 1;

            }
        }
        if(level ==0) {
            // menu selection
            if (key == XK_Return) {
                if(menuSelection==0) {
                    // play sound for menu selection then for init sequence
#ifdef USE_SOUND
                    fmod_playsound(button3);
                    fmod_playsound(electronicNoise);
#endif
                    // make sure hero is not shooting immediately
                    level=-1;
                    lastFacing = 0;
                }
                if(menuSelection==1 or menuSelection==2 or menuSelection==2 or menuSelection==4) {
#ifdef USE_SOUND
                    fmod_playsound(bleep);
#endif
                    showInvalid = 1;
                }
                if(menuSelection==3) {
#ifdef USE_SOUND
                    fmod_playsound(bleep);
#endif
                    showInvalid = 0;
                    return 1;
                }
            }
            if ( key == XK_Down){
                showInvalid = 0;
                if(menuSelection ==0) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection =1;
                }
                else if(menuSelection ==1) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection =0;
                }
                else if(menuSelection ==3) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection = 4;
                }
                else if(menuSelection ==4) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection = 3;
                }
            }
            if ( key == XK_Up){
                showInvalid = 0;
                if(menuSelection ==0) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection =1;
                }
                else if(menuSelection ==4) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection = 3;
                }
                else if(menuSelection ==1) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection =0;
                }
                else if(menuSelection ==3) {
#ifdef USE_SOUND
                    fmod_playsound(tick);
#endif
                    menuSelection = 4;
                }
            }
            if ( key == XK_Right){
#ifdef USE_SOUND
                fmod_playsound(tick);
#endif
                showInvalid = 0;
                if(menuSelection ==0)
                    menuSelection = 4;
                else if(menuSelection ==1)
                    menuSelection = 2;
                else if(menuSelection ==2)
                    menuSelection = 3;
                else if(menuSelection ==3)
                    menuSelection=1;
                else if(menuSelection ==4)
                    menuSelection=0;
            }
            if ( key == XK_Left){
#ifdef USE_SOUND
                fmod_playsound(tick);
#endif
                showInvalid = 0;
                if(menuSelection ==0)
                    menuSelection = 4;
                else if(menuSelection ==1)
                    menuSelection = 3;
                else if(menuSelection ==2)
                    menuSelection = 1;
                else if(menuSelection ==3)
                    menuSelection=2;
                else if(menuSelection ==4)
                    menuSelection=0;
            }
        }
    }
    else if (e->type == KeyRelease) {
        if ((key == XK_a || key == XK_Left)) {
            hero->stop();
        }
        if ((key == XK_d || key == XK_Right)) {
            hero->stop();
        }
        if (key == XK_space) {
            hero->setShooting(false);
        }
    }

    return 0;
}

void makePlatform(int w, int h, int x, int y) {
    grounds[grounds_length] = new Platform();
    grounds[grounds_length]->insert("./images/level.ppm", 1, 1);
    grounds[grounds_length]->init(w, h, x, y);
    grounds[grounds_length]->setupTile();
    grounds_length++;
}

void makeItems(int w, int h, int x, int y) {
    itemsHold[items_length] = new Item();
    itemsHold[items_length]->setID(items_length);
    itemsHold[items_length]->insert("./images/firemon.ppm", 1, 1);
    itemsHold[items_length]->setSize(16, 20);
    itemsHold[items_length]->init(16, 20, x, y);
    items_length++;
}

void deleteItem(int id) {
    if (items_length <= 0) return;
    if (id < 0) return;

    itemsHold[id] = new Item();
    delete itemsHold[id];

    for (int i = id; i < items_length-1; i++) {
        itemsHold[i] = itemsHold[i + 1];
        itemsHold[i]->setID(itemsHold[i]->getID()-1);
    }
    itemsHold[items_length-1] = new Item();
    delete itemsHold[items_length-1];

    items_length--;
}

void makeEnemy(int w, int h, Object *ground, int type) {
    if (enemies_length<MAX_ENEMIES){
        switch (type){
            case 1:
                enemies[enemies_length] = new Enemy(w, h, ground, type); 
                enemies[enemies_length]->insert("./images/enemy1.ppm", 26, 1);
                enemies[enemies_length]->setBottom(-44);
                enemies[enemies_length]->setLeft(-24);
                enemies[enemies_length]->setRight(24);
                enemies[enemies_length]->setTop(24);
                enemies[enemies_length]->setHeight(25);
                break;
            case 2:
                enemies[enemies_length] = new Enemy(w, h, ground, type); 
                enemies[enemies_length]->insert("./images/enemy2_1.ppm", 26, 1);
                /*enemies[enemies_length]->setBottom(-44);
                  enemies[enemies_length]->setLeft(-24);
                  enemies[enemies_length]->setRight(24);
                  enemies[enemies_length]->setTop(24);
                  enemies[enemies_length]->setHeight(25);
                  */
                break;
        }
        enemies_length++;
    }
    else{
        cout << "Enemies array full!" << endl;
    }
}

bool detectCollide (Object *obj, Object *ground) {
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (obj->getRight() > ground->getLeft() &&
            obj->getLeft()   < ground->getRight() &&
            obj->getBottom() < ground->getTop()  &&
            obj->getTop()    > ground->getBottom());
}

bool detectItem (Object *obj, Item *targetItem) {
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    if (obj->getRight() > targetItem->getLeft() &&
            obj->getLeft()   < targetItem->getRight() &&
            obj->getBottom() < targetItem->getTop()  &&
            obj->getTop()    > targetItem->getBottom()) {
        cout << "Item touched\n";
        targetItem->causeEffect(hero);
        deleteItem(obj->getID());
        return true;
    }
    return false;
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
            //cout << "set ground" << obj << endl;
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
    hero->autoSet();
    if(hero->getHealth() <= 0) hero->stop();  

    //Detect Collisions
    for (i=0; i<grounds_length; i++) {
        groundCollide(hero, grounds[i]);
    } 

    hero->jumpRefresh();
    hero->cycleAnimations();
    hero->autoState();//This set the isStuff like isWalking, tmp fix
    hero->setVelocityY( hero->getVelocityY() - GRAVITY);

    // Cycle through hero index sequences

    // remove a life when the hero falls off cliff
    if (hero->getCenterY() < 0){
        if (!(hero->isDying)) {
            hero->decrementLives();
        }
        hero->setHealth(0);
    }

    if (hero->getHealth()<=0) {//Going to try to Mimic The Death Function. Heres a tmp fix though
        hero->stop();
        if (!(hero->isDying)) {
            hero->isDying=1;
            gettimeofday(&seqStart, NULL);
#ifdef USE_SOUND
            fmod_playsound(dunDunDun);
#endif
        }
        else{
            gettimeofday(&seqEnd, NULL);
            if (((diff_ms(seqEnd, seqStart)) > 1000)) {
                hero->setCenter(HERO_START_X, HERO_START_Y);
                hero->isDying=0;
                hero->repairHealth(100); 
            }
        }
    }

    //Detect Item
    for (j=0; j < items_length; j++) {
        detectItem(hero, itemsHold[j]);
    }

    //Bullet creation
    Bullet *b;
    if (hero->checkShooting()){
        gettimeofday(&fireEnd, NULL);
        if (((diff_ms(fireEnd, fireStart)) > 250)) { //Fire rate 250ms
            gettimeofday(&fireStart, NULL); //Reset firing rate timer
            makeBullet(hero->getCenterX(), hero->getCenterY()+15, (hero->checkMirror()?-18:18), 2);
            /*b = new Bullet;
            b->pos[0] = hero->getCenterX();
            b->pos[1] = hero->getCenterY()+15;
            */
#ifdef USE_SOUND
            fmod_playsound(mvalSingle);
#endif
            /*
            //if (lastFacing or hero->getVelocityX()<0) {
            //    b->vel[0] = -18;
            //} else {
            if (hero->checkMirror()){
                b->vel[0] = -18;
            } else {
                b->vel[0] = 18;
            }
            b->type = 2;
            b->vel[1] = 0;
            b->color[0] = 1.0f;
            b->color[1] = 1.0f;
            b->color[2] = 1.0f;
            b->next = bulletHead;
            if (bulletHead != NULL)
                bulletHead->prev = b;
            bulletHead = b;
            bullets++;
            */
        }
        
        }

        //bullet collisions against grounds
        b = bulletHead;
        while (b) {
            //move the bullet
            b->pos[0] += b->vel[0];
            b->pos[1] += b->vel[1];

            for (i=0;i<grounds_length;i++){
                if (bulletCollide(b,grounds[i])){
                    deleteBullet(b);
                    break;
                }
            }
            //Check for collision with window edges
            if (b->pos[0] > WINDOW_WIDTH+roomX or b->pos[0] < roomX-WINDOW_WIDTH) {
                    deleteBullet(b);
            }
            if (b)
                b = b->next;
        }

        // Enemy movement, enemy ai
        for (i=0;i<enemies_length;i++) {
            enemies[i]->setOldCenter();
            enemies[i]->enemyAI(hero); //Where does enemy go?
            //enemyAI(enemies[i]);
            //bullets
            b = bulletHead;
            while (b) {
                if ((b->type==2) && (bulletCollide(b,enemies[i]))){
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
    Object createAI (int w, int h, Object *ground) {
        Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
        return newEnemy;

    }

    bool inWindow(Object &obj) {
        return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
                    obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or
                (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
                 obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
    }

    void moveWindow () {
        double heroWinPosX = hero->getCenterX();
        double heroWinPosY = hero->getCenterY();
        int interval=120;

        //move window forward
        if ((heroWinPosX > roomX + interval) && ((roomX+WINDOW_HALF_WIDTH)<LEVEL_WIDTH-6)) {
            roomX+=7;
        }
        //move window backward (fast move if hero is far away)
        else if ((heroWinPosX < roomX - interval) && roomX>(WINDOW_HALF_WIDTH+6)) {
            roomX-=7;
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
        renderAnimations(x, y);
        renderItems(x, y);
        renderLives();
        renderHealthBar();

        if (fail>0) {
            writeWords("CRITICAL FAILURE", WINDOW_WIDTH/2- 200, WINDOW_HEIGHT/2);
            fail--;
        }
    }

    void renderAnimations(int x, int y){
        if(animateOn == 0) return;

        glPushMatrix();
        glTranslatef(- x + 350, - y + 350, 0);
        //explode.cycleAnimations();
        //explode.drawBox();


        glEnd(); glPopMatrix(); 
    }

    void renderItems(int x, int y){
        for (i=0;i<items_length;i++) {
            if (inWindow(*(itemsHold[i]))) {
                glPushMatrix();
                glTranslatef(- x, - y, 0);
                //items->drawBox();
                itemsHold[i]->drawBox();
                glEnd(); glPopMatrix(); 
            }
        }
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
        for (i=0;i< storeIn.grounds_length ;i++) {
            glPushMatrix();
            glTranslatef(- x, - y, 0);
            storeIn.grounds[i].drawRow(0,0);
            glEnd(); glPopMatrix();
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
        //Easy Drawing
        glPushMatrix();
        glTranslatef(-x, -y, 0);
        hero->drawBox();
        glPopMatrix();
    }

    void renderBullets (int x, int y) {
        //Draw the bullets
        Bullet *b = bulletHead;
        while (b) {
            //Log("draw bullet...\n");
            /*glColor3f(1.0, 1.0, 1.0);
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
              */
            glPushMatrix();
            if (b->vel[0]>0)
                glTranslatef(b->pos[0]-x, b->pos[1]-y, 0);
            else // flip bullet TO-DO
                glTranslatef(b->pos[0]-x, b->pos[1]-y, 0);
            bulletImage.drawTile(b->type,0);
            glPopMatrix();

            b = b->next;
        }
    }

    void renderInitMenu () {
        int frameTime = 70;
        gettimeofday(&frameEnd, NULL);

        // loop through frames
        if (diff_ms(frameEnd, frameStart) > frameTime) { 
            gettimeofday(&frameStart, NULL);
            frameIndex++;
        }
        // transition to render level 1
        if (frameIndex == 65) {
#ifdef USE_SOUND
            fmod_playsound(megamanTheme);
#endif
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
    void renderLives () {
        int w = 50;
        int h = 50;

        // prepare opengl
        glPushMatrix();
        glColor3ub(0,100,40);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        // remove GL_GREATER function to have black background
        glAlphaFunc(GL_GREATER, 0.1f);
        glBindTexture(GL_TEXTURE_2D, lifeTexture[0]);
        glColor4ub(255,255,255,255);

        // player begins with 3 lives, but has opportunities to earn 
        // up to 2 extra lives, so may have 5 total at one point

        int lives = hero->getLives();

        for(int k=0; k<lives; k++) {
            glBegin(GL_QUADS);
            glTexCoord2f(0, 1) ; glVertex2i( k*w,    0);
            glTexCoord2f(0, 0) ; glVertex2i( k*w,    h);
            glTexCoord2f(1, 0) ; glVertex2i((k+1)*w, h);
            glTexCoord2f(1, 1) ; glVertex2i((k+1)*w, 0);
            glEnd();
        }
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);

    }

    void renderHealthBar () {
        //int WW = WINDOW_WIDTH; // not used warning
        int WH = WINDOW_HEIGHT;
        int h = 30;
        int w = 200;
        float row_size = 0.2;

        // prepare opengl
        glPushMatrix();
        glColor3ub(0,100,40);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_ALPHA_TEST);
        // remove GL_GREATER function to have black background
        // glAlphaFunc(GL_GREATER, 0.1f);
        glBindTexture(GL_TEXTURE_2D, healthBarTexture[0]);
        glColor4ub(255,255,255,255);

        // tile index ranges from 1 to 5
        float index = healthIndex;

        glBegin(GL_QUADS);
        glTexCoord2f(0, row_size*index);          glVertex2i(0, WH-h);
        glTexCoord2f(0, row_size*index-row_size); glVertex2i(0, WH-10);
        glTexCoord2f(1, row_size*index-row_size); glVertex2i(w, WH-10);
        glTexCoord2f(1, row_size*index);          glVertex2i(w, WH-h);
        glEnd();
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);
    }

    void renderComputerScreenMenu () {
        gettimeofday(&frameEnd, NULL);
        int frameTime = 190;
        int WHW = WINDOW_HALF_WIDTH;
        int WHH = WINDOW_HALF_HEIGHT;

        // loop through frames
        if (diff_ms(frameEnd, frameStart) > frameTime) {
            gettimeofday(&frameStart, NULL);
            frameIndex++;
        }

        //reset frame sequence
        if (frameIndex == 26) {
            frameIndex = 0;
        }

        glPushMatrix();
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glTranslatef(WHW, WHH, 0);
        glColor3ub(0,100,40);

        //draw back sequence of computer images
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, computerScreenTextures[frameIndex]);
        glColor4ub(255,255,255,255);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1) ; glVertex2i(-WHW,-WHH);
        glTexCoord2f(0, 0) ; glVertex2i(-WHW, WHH);
        glTexCoord2f(1, 0) ; glVertex2i( WHW, WHH);
        glTexCoord2f(1, 1) ; glVertex2i( WHW,-WHH);
        glEnd(); 
        glDisable(GL_TEXTURE_2D);

        // draw highlighted box next based on current menu selection
        glColor3ub(120,200,100);
        glBegin(GL_QUADS);
        switch(menuSelection) {
            case 0:
                // top left corner
                glVertex2i(-WHW, 50);
                glVertex2i(-WHW, WHW);
                glVertex2i( -210, WHW);
                glVertex2i( -210, 50);
                break;
            case 1:
                // bottom left corner
                glVertex2i(-WHW, -WHW+230);
                glVertex2i(-WHW, 50);
                glVertex2i( -150, 50);
                glVertex2i( -150, -WHW+230);
                break;
            case 2:
                // bottom center
                glVertex2i(-150, -WHW+280);
                glVertex2i(-150, 50);
                glVertex2i( 190, 50);
                glVertex2i( 190, -WHW+280);
                break;
            case 3:
                // bottom right corner
                glVertex2i(WHW, -WHW+240);
                glVertex2i(190, -WHW+240);
                glVertex2i( 190, 50);
                glVertex2i( WHW, 50);
                break;
            case 4:
                // top right corner
                glVertex2i(WHW, WHW);
                glVertex2i(190, WHW);
                glVertex2i( 190, 50);
                glVertex2i( WHW, 50);
                break;
        }
        glEnd();

        //draw top sequence of computer images, filtering out green to show
        //highlighted box through
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_TEXTURE_2D);
        glAlphaFunc(GL_LESS, 0.2f);
        glColor4ub(255,255,255,255);
        glBegin(GL_QUADS);

        glTexCoord2f(0, 1) ; glVertex2i(-WHW,-WHH);
        glTexCoord2f(0, 0) ; glVertex2i(-WHW, WHH);
        glTexCoord2f(1, 0) ; glVertex2i( WHW, WHH);
        glTexCoord2f(1, 1) ; glVertex2i( WHW,-WHH);

        glEnd(); glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_ALPHA_TEST);

        writeWords("CRITICAL FAILURE", WINDOW_WIDTH/2- 205, WINDOW_HEIGHT/2 + 230);
        writeWords("--- selection ---", WINDOW_WIDTH/2- 205, WINDOW_HEIGHT/2 + 130);
        switch(menuSelection) {
            case 0:
                writeWords("       RAM", WINDOW_WIDTH/2- 205, WINDOW_HEIGHT/2 + 95);
                break;
            case 1:
                writeWords("       CPU", WINDOW_WIDTH/2- 190, WINDOW_HEIGHT/2 + 95);
                break;
            case 2:
                writeWords("  MOTHER BOARD", WINDOW_WIDTH/2- 190, WINDOW_HEIGHT/2 + 95);
                break;
            case 3:
                writeWords("  /EXIT SYSTEM/", WINDOW_WIDTH/2- 190, WINDOW_HEIGHT/2 + 95);
                break;
            case 4:
                writeWords("   HARD DRIVE", WINDOW_WIDTH/2- 190, WINDOW_HEIGHT/2 + 95);
                break;
        }

        // user tried to select a level that was not yet unlocked
        if(showInvalid) {
            writeWords("error requires ram!", WINDOW_WIDTH/2- 205, WINDOW_HEIGHT/2 + 190);
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
        //cout << "deleting bullet..." << endl;
        if (!node){
            //cout << "NULL BULLET!!!" << endl;
            return;
        }
        if (node->prev == NULL) {
            if (node->next == NULL) {
                bulletHead = NULL;
            } else {
                node->next->prev = NULL;
                bulletHead = node->next;
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

