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
#include <fstream>

#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "fmod.h"

//#include "Sprite.cpp"
#include "bethanyA.cpp"

//#include "fastFont.h"
#include "tedP.cpp"
#include "Player.h"

//Platforms/Grounds
#include "chadD.cpp"

//Enemies
#include "brianS.cpp"

#include "Storage.cpp"

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

// 1 to use tool editor, 0 to use pre-defined objects
#define USE_TOOLS 0

#define MAX_BACKGROUND_BITS 6000
#define HERO_START_X 150
#define HERO_START_Y 350

using namespace std;

typedef double Vec[3];

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
int menuSelection = 0;

//Game Globals
string str = "";
bgBit *bitHead = NULL;
Bullet *bhead = NULL;
Enemy *enemies[MAX_ENEMIES];
Object *hero; // Class Player
Player *testHero;
Platform *grounds[MAX_GROUNDS];
int bg, bullets, grounds_length, enemies_length,  i, j, level=0;
int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;
int fail=0;
int interval=120;
double g_left, g_right, g_top, g_bottom;
int quit=0;
int showInvalid=0;

//Images and Textures
Ppmimage *heroImage=NULL;
GLuint heroTexture;

Ppmimage *initImages[32];
GLuint initTextures[65];

Ppmimage *computerScreenImages[32];
GLuint computerScreenTextures[32];

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void init_sounds(void);
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
  initXWindows(); init_opengl(); init_sounds();

  //declare hero object
  hero = new Object(46, 48, HERO_START_X, HERO_START_Y);
  hero->setTop(40);
  hero->setBottom(-44);
  hero->setLeft(-26);
  hero->setRight(26);

  testHero = new Player();
  testHero->insert("./images/hero.ppm", 13, 1);
  testHero->setSize(44,48);

  frameIndex = 0;

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
    glGenTextures(65, initTextures);
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

  if( USE_TOOLS ){
      // loads platform objects from file
      ifstream dfs("test.ros", ios::binary);
      dfs.read((char *)&storeIn, sizeof(storeIn));

      // pulls in created objects from store
      cout << "what is the length in storeIn " << storeIn.grounds_length << "\n";
      for(int i = 0; i < storeIn.grounds_length; i++){
        grounds[i] = &storeIn.grounds[i];
        grounds[i]->reInitSprite();
        //storeIn.grounds[i].reInitSprite();
        grounds_length++;
      }
  }
  // don't use the tools, use pre defined objects
  else {
  //storeIn.grounds_length = 0;
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
  }

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

}
void init_sounds() {
    //FMOD_RESULT result;
    if (fmod_init()) {
        std::cout << "ERROR - fmod_init()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/tick.wav", 0)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/drip.wav", 1)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    fmod_setmode(0,FMOD_LOOP_OFF);
    //fmod_playsound(0);
    //fmod_systemupdate();
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
void highlightBox(int x, int y) {
  cout << "x: " << x << "y: " << y <<endl;
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
        testHero->jump();
      }
      // move character left
      if (key == XK_a || key == XK_Left) {
        testHero->moveLeft();
      }
      // move character right
      if (key == XK_d || key == XK_Right){
        testHero->moveRight();
      }
      // shooting
      if (key == XK_space) {
        testHero->setShooting(true);
        hero->isShooting=1;
      }
      // debug death
      if (key == XK_y) {
        testHero->setHealth(0);
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
      // play sounds for debugging
      if (key == XK_t) {
          fmod_playsound(0);
      }
    }
    if(level ==0) {
      // menu selection
      if (key == XK_Return) {
        if(menuSelection==0) {
          level=-1;
          lastFacing = 0;
        }
        if(menuSelection==1 or menuSelection==2 or menuSelection==4)
          showInvalid = 1;
        if(menuSelection==3) {
          showInvalid = 0;
          return 1;
        }
      }
      if ( key == XK_Down){
        showInvalid = 0;
        if(menuSelection ==0)
          menuSelection =1;
        else if(menuSelection ==1)
          menuSelection =0;
        else if(menuSelection ==3)
          menuSelection = 4;
        else if(menuSelection ==4)
          menuSelection = 3;
      }
      if ( key == XK_Up){
        showInvalid = 0;
        if(menuSelection ==0)
          menuSelection =1;
        else if(menuSelection ==4)
          menuSelection = 3;
        else if(menuSelection ==1)
          menuSelection =0;
        else if(menuSelection ==3)
          menuSelection = 4;
      }
      if ( key == XK_Right){
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
    if ((key == XK_a || key == XK_Left) && !(hero->isDying)) {
      testHero->stop();
    }
    if ((key == XK_d || key == XK_Right) && !(hero->isDying)) {
      testHero->stop();
    }
    if (key == XK_space) {
      testHero->setShooting(false);
    }
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

bool detectCollide2 (Object *obj, Object ground) {
  //Gets (Moving Object, Static Object)
  //Reture True if Moving Object Collides with Static Object
  return (obj->getRight() > ground.getLeft() &&
      obj->getLeft()   < ground.getRight() &&
      obj->getBottom() < ground.getTop()  &&
      obj->getTop()    > ground.getBottom());
}

void groundCollide2 (Object *obj, Object ground) {
  //(Moving Object, Static Object)
  //Detects Which boundaries the Moving Object is around the Static Object
  //top,down,left,right
  if (detectCollide2(obj, ground)) {
    h_right=obj->getRight();
    h_left=obj->getLeft();
    h_top=obj->getTop();
    h_bottom=obj->getBottom();
    g_right=ground.getRight();
    g_bottom=ground.getBottom();
    g_top=ground.getTop();
    g_left=ground.getLeft();
    //If moving object is on top of the static object
    if (!(obj->getOldBottom() < g_top) && !(h_bottom >= g_top) && (obj->getVelocityY() < 0)) {
      obj->setVelocityY(0);
      obj->setCenter(obj->getCenterX(), g_top+(obj->getCenterY()-h_bottom));
      //obj->setFloor(ground);
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

  testHero->setOldCenter();
  testHero->autoSet();
  if(testHero->getHealth() <= 0) testHero->stop();  

  //Detect Collisions
  for (i=0; i<grounds_length; i++) {
    groundCollide(hero, grounds[i]);
    groundCollide(testHero, grounds[i]);
  } 
  for (i=0; i<storeIn.grounds_length; i++) {
    groundCollide2(testHero, storeIn.grounds[i]);
  }

  testHero->jumpRefresh();
  testHero->cycleAnimations();
  testHero->autoState();//This set the isStuff like isWalking, tmp fix
  testHero->setVelocityY( testHero->getVelocityY() - GRAVITY);

  // Cycle through hero index sequences

  if (testHero->getCenterY() < 0){ life = 0; testHero->setHealth(0);}

  if (life<=0) {//Going to try to Mimic The Death Function. Heres a tmp fix though
    testHero->stop();
    if (!(testHero->isDying)) {
      testHero->isDying=1;
      gettimeofday(&seqStart, NULL);
      fail=100;
    }
    else{
      gettimeofday(&seqEnd, NULL);
      if (((diff_ms(seqEnd, seqStart)) > 1000)) {
        testHero->setCenter(HERO_START_X, HERO_START_Y);

        testHero->isDying=0;
        testHero->setHealth(100); life=100; lives--;
      }
    }
  }

  //Bullet creation
  Bullet *b;
  if (testHero->checkShooting()){
    gettimeofday(&fireEnd, NULL);
    if (((diff_ms(fireEnd, fireStart)) > 250)) { //Fire rate 250ms
      gettimeofday(&fireStart, NULL); //Reset firing rate timer
      b = new Bullet;
      b->pos[0] = testHero->getCenterX();
      b->pos[1] = testHero->getCenterY()+15;
      //if (lastFacing or testHero->getVelocityX()<0) {
      //    b->vel[0] = -18;
      //} else {
      if (testHero->checkMirror()){
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
      enemies[i]->enemyAI(testHero); //Where does enemy go?
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
    int frameTime = 70;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) { frameIndex++;
      gettimeofday(&frameEnd, NULL);
    }

    if (frameIndex == 65) {
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
  void renderComputerScreenMenu () {
    gettimeofday(&frameStart, NULL);
    int frameTime = 190;

    // loop through frames
    if (diff_ms(frameStart, frameEnd) > frameTime) {
        frameIndex++;
          gettimeofday(&frameEnd, NULL);
    }

    if (frameIndex == 26) {
      //reset frame sequence
      frameIndex = 0;
    }

    glPushMatrix();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glTranslatef(WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, 0);
    glColor3ub(0,100,40);

    int WHW = WINDOW_HALF_WIDTH;

    //draw sequence of computer images
   // glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, computerScreenTextures[frameIndex]);
//    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

    glEnd(); //glPopMatrix();
    glDisable(GL_TEXTURE_2D);
   // glDisable(GL_ALPHA_TEST);

    glColor3ub(120,200,100);
    // draw highlighted portion of menu based on current selection
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
    glEnd(); //glPopMatrix();

    //draw sequence of computer images
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_TEXTURE_2D);
    glAlphaFunc(GL_LESS, 0.2f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1) ; glVertex2i(-WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);
    glTexCoord2f(0, 0) ; glVertex2i(-WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 0) ; glVertex2i( WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
    glTexCoord2f(1, 1) ; glVertex2i( WINDOW_HALF_WIDTH,-WINDOW_HALF_HEIGHT);

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

    if(showInvalid) {
      writeWords("error requires ram!", WINDOW_WIDTH/2- 205, WINDOW_HEIGHT/2 + 190);
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
      //if (inWindow(*(storeIn.grounds[i]))) {
        //Platform
        glPushMatrix();
        glTranslatef(- x, - y, 0);
        storeIn.grounds[i].drawRow(0,0);
        glEnd(); glPopMatrix();
      //}
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
    testHero->drawBox();
    glPopMatrix();
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
    double heroWinPosX = testHero->getCenterX();
    double heroWinPosY = testHero->getCenterY();

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

