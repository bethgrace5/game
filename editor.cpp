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

//#include "Sprite.cpp"
#include "bethanyA.cpp"

//#include "fastFont.h"
#include "tedP.cpp"
#include "Player.h"

//Platforms/Grounds
#include "chadD.cpp"

//Enemies
#include "brianS.cpp"

#include <fstream>

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

//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Game Globals
string str = "";
Enemy *enemies[MAX_ENEMIES];
Object *hero; // Class Player
Player *testHero;
Platform *grounds[MAX_GROUNDS];
int bg, bullets, grounds_length, enemies_length,  i, j, level=-1;
int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;
int interval=120;
double g_left, g_right, g_top, g_bottom;
int quit=0;

struct data{
  Platform grounds[MAX_GROUNDS];
  int grounds_length;
} storeIn;

bool create = 0;
bool selecter = 0;
int  holdID = -1;
int saveID = -1;
int id = 0;

int mouse(int x, int y);
void draging(int x, int y);
void incRow();
void incColumn();


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
void renderGrounds(int x, int y);
void deleteEnemy(int ind);
void renderEnemies(int x, int y);
void renderHero(int x, int y);
void makePlatform(int w, int h, int x, int y); 
void makeEnemy(int w, int h, Object *ground, int type); 
Object createAI( int w, int h, Object *ground);

bool detectCollide(Object *obj, Object *ground);

bool inWindow(Object &obj) {
  return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
        obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or
      (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
       obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
}


int main(void) {
  initXWindows(); init_opengl();

  //declare hero object
  testHero = new Player();
  testHero->insert("./images/hero.ppm", 13, 1);
  testHero->setSize(44,48);

  if(QUICK_LOAD_TIME) {
    level = 1;
  }

  while (!quit) { //Staring Animation
    while (XPending(dpy)) {
      //Player User Interfaces
      XEvent e; XNextEvent(dpy, &e);
      check_mouse(&e);
      quit = check_keys(&e);
    }
    movement(); render(); 
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

}

void makePlatform(int w, int h, int x, int y) {
  cout << "Make Ground \n";
  //storeIn.grounds[storeIn.grounds_length] = new Platform();
  storeIn.grounds[storeIn.grounds_length].insert("./images/level.ppm", 1, 1);
  storeIn.grounds[storeIn.grounds_length].init(w, h, x, y);
  storeIn.grounds[storeIn.grounds_length].setupTile();
  storeIn.grounds[storeIn.grounds_length].setID(id);
  id++;
  std::cout << "What is " << storeIn.grounds[storeIn.grounds_length].getID() <<
    std::endl;
  storeIn.grounds_length++;
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

static int savex = 0, savey = 0;
void check_mouse (XEvent *e) {
  //static int n = 0;
  if (e->type == ButtonRelease) { 
    std::cout << " Release\n";
    holdID = -1; 

    return;
  }

  int take;
  if (e->type == ButtonPress) {
    if (e->xbutton.button==1) { //Left button was pressed
      if(create == 1){
        std::cout << " x " << savex << ", y " << savey << "\n";
        makePlatform(30, 30, savex, savey);
        return;
      }
      if(selecter == 1){
        take = mouse(savex, savey); 
        std::cout << "The Id is: " << take << "\n";
        saveID = holdID = take;
      }
    }
    if (e->xbutton.button==2) { //Right button was pressed
      take = mouse(savex, savey);
      std::cout << "The Id is: " << take << "\n";

      return;
    }
    if (e->xbutton.button==3) { //Right button was pressed
      take = mouse(savex, savey);
      std::cout << "The Id is: " << take << "\n";

      return;
    }
  }
  if (e->xbutton.button==3) { //Right button was pressed
    return;
  }

  //Did the mouse move?
  if (savex != e->xbutton.x || savey != e->xbutton.y) {
    int x = roomX - WINDOW_HALF_WIDTH;
    //int y = roomY - WINDOW_HALF_HEIGHT;
    //int y = WINDOW_HEIGHT - e->xbutton.y;
    savex = e->xbutton.x + x; //xpast = savex;
    savey  = WINDOW_HEIGHT - e->xbutton.y;
  }
}
int mouse(int x, int y){
  for(int i = 0; i < storeIn.grounds_length; i++){
    if(storeIn.grounds[i].getRight()  > x &&
        storeIn.grounds[i].getLeft()   < x &&
        storeIn.grounds[i].getBottom() < y &&
        storeIn.grounds[i].getTop()    > y){
      return storeIn.grounds[i].getID(); 
    }
  }
  return -1;
}

void draging(int x, int y){
  if(holdID < 0) return;
  storeIn.grounds[holdID].setCenter(x, y);
}
void incRow(){
  if(saveID < 0) return;
  storeIn.grounds[saveID].init(storeIn.grounds[saveID].getWidth()+30,
                               storeIn.grounds[saveID].getHeight());
  storeIn.grounds[saveID].setupTile();
}
void incColumn(){
  if(saveID < 0) return;
  storeIn.grounds[saveID].init(storeIn.grounds[saveID].getWidth(),
                               storeIn.grounds[saveID].getHeight()+30);
  storeIn.grounds[saveID].setupTile();
}

int check_keys (XEvent *e) {
  //handle input from the keyboard
  int key = XLookupKeysym(&e->xkey, 0);
  if (e->type == KeyPress) {
    if (key == XK_Escape or key == XK_q) return 1;
    if( key == XK_s){
      std::cout << "Saving \n";
      ofstream dfs("test.ros", ios::binary); 
      dfs.write((char *)&storeIn, sizeof(storeIn));
    }
    if( key == XK_l){
      std::cout << "Loading \n";  
      ifstream dfs("test.ros", ios::binary);
      std::cout << "what is the sizeOf(storeIn)" << sizeof(storeIn) << "\n";
      dfs.read((char *)&storeIn, sizeof(storeIn));
    }
    if( key == XK_y){
      for(int i = 0; i < storeIn.grounds_length; i++){
        storeIn.grounds[i].reInitSprite(); 
      } 
    }
    if(key == XK_c){
      create = 1; selecter = 0; 
    }
    if(key == XK_v){
      create = 0; selecter = 1;  
    }
    if(key == XK_r){
      incRow();  
    }
    if(key == XK_t){
      incColumn(); 
    }

    if(key == XK_a) roomX -= 50;
    if(key == XK_d) roomX += 50;
    /*
       if ((key == XK_w || key == XK_Up)); 
       if (key == XK_a || key == XK_Left);
       if (key == XK_d || key == XK_Right);
       if (key == XK_space) ;
       if (key == XK_y) {}
       */
  }

  else if (e->type == KeyRelease) {
    /*
       if ((key == XK_a || key == XK_Left) && !(hero->isDying));
       if ((key == XK_d || key == XK_Right) && !(hero->isDying));
       if (key == XK_space);*/
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

void movement() {
  draging(savex, savey);
  // Hero Apply Velocity, Add Gravity
}

void renderGrounds (int x, int y) {
  // render grounds
  for (i=0;i<storeIn.grounds_length;i++) {
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

void render () {
  int x = roomX - WINDOW_HALF_WIDTH;
  int y = roomY - WINDOW_HALF_HEIGHT;
  glClear(GL_COLOR_BUFFER_BIT);
  // Draw Background Falling Bits
  renderGrounds(x, y);
  renderEnemies(0, 0);
  renderHero(x, y);
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

Object createAI (int w, int h, Object *ground) {
  Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
  return newEnemy;
}

