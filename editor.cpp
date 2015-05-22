/*
======================+
    Tool Editor       |
======================+
  Main Center         |
  Platform Editor     |
  Enemy Editor        |
  Mouse Check         |
  Key Check           |
  Physics             |
  Drawing             |
  Storage Editor      |
  Window Setup        |
======================+
*/
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

#define MAX_BACKGROUND_BITS 6000
#define HERO_START_X 150
#define HERO_START_Y 350

#define OPTIONAL_STORAGE 1 //If 1, you can write the name of the save file

using namespace std;

typedef double Vec[3];

//X Windows variables
Display *dpy; Window win; GLXContext glc;
//------------------------------
//Game Globals
//------------------------------
Player *testHero;
Enemy *enemies[MAX_ENEMIES];
int grounds_length, enemies_length;
int i, j;

int roomX=WINDOW_HALF_WIDTH;
int roomY=WINDOW_HALF_HEIGHT;
int quit=0;

bool saving;
bool loading;

bool create = 0;
bool selecter = 0;
int  holdID = -1;
int saveID = -1;
int id = 0;
//------------------------------
//Function Prototype
//------------------------------
int clickObject(int x, int y);
void draging(int x, int y);

void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void moveWindow(void);

void save();
void load();

void check_mouse(XEvent *e);
int  check_keys (XEvent *e);

void makePlatform(int w, int h, int x, int y); 
void setRow(int size);
void setColumn(int size);

void renderHero(int x, int y);

void makeEnemy(int w, int h, Object *ground, int type); 
Object createAI( int w, int h, Object *ground);
void deleteEnemy(int ind);


void render(void);
void renderEnemies(int x, int y);
void renderGrounds(int x, int y);
void renderSave();
void renderLoad();
void renderBox();

void movement(void);
bool detectCollide(Object *obj, Object *ground);

bool inWindow(Object &obj) {
  return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
        obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or
      (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
       obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
}

//====================================================================
//    Main Center
//====================================================================
int main(void) {
  initXWindows(); init_opengl();

  //declare hero object
  testHero = new Player();
  testHero->insert("./images/hero.ppm", 13, 1);
  testHero->setSize(44,48);

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

//=====================================================================
//  Platform Editor
//=====================================================================
void makePlatform(int w, int h, int x, int y) {
  cout << "Make Ground \n";
  //storeIn.grounds[storeIn.grounds_length] = new Platform();
  storeIn.grounds[storeIn.grounds_length].insert("./images/level.ppm", 1, 1);
  storeIn.grounds[storeIn.grounds_length].init(w, h, x, y);
  //The Above Function is whats casuing it but
  storeIn.grounds[storeIn.grounds_length].setupTile();
  storeIn.grounds[storeIn.grounds_length].setID(id);
  id++;
  std::cout << "What is " << storeIn.grounds[storeIn.grounds_length].getID() <<
    std::endl;
  storeIn.grounds_length++;
}
void setRow(int size){
  if(saveID < 0) return;
  int changeBy = 1; if(size < 0) changeBy = -1;
  int spriteWidth = storeIn.grounds[saveID].getClipWidth() * changeBy;
  int currentWidth = storeIn.grounds[saveID].getWidth();
  storeIn.grounds[saveID].setWidth(currentWidth + spriteWidth);
  storeIn.grounds[saveID].setupTile();
}

void setColumn(int size){
  if(saveID < 0) return;
  int changeBy = 1; if(size < 0) changeBy = -1;
  int spriteHeight  = storeIn.grounds[saveID].getClipHeight() * changeBy;
  int currentHeight = storeIn.grounds[saveID].getHeight();
  storeIn.grounds[saveID].setHeight(spriteHeight + currentHeight);
  storeIn.grounds[saveID].setupTile();
}
//=====================================================================
//  Enemy Editor
//=====================================================================
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
//=====================================================================
//  Mouse Check
//=====================================================================
static int savex = 0, savey = 0;
void check_mouse (XEvent *e) {
  //static int n = 0;
  if (e->type == ButtonRelease) { 
    std::cout << " Release\n";
    holdID = -1;  return;
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
        take = clickObject(savex, savey); 
        std::cout << "The Id is: " << take << "\n";
        saveID = holdID = take;
      }
    }
    if (e->xbutton.button==2) {

      return;
    }
    if (e->xbutton.button==3) {

      return;
    }
  }
  if (e->xbutton.button==3) {
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

int clickObject(int x, int y){
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
//=====================================================================
//  Key Check
//=====================================================================
int check_keys (XEvent *e) {
  //handle input from the keyboard
  int key = XLookupKeysym(&e->xkey, 0);
  if (e->type == KeyPress) {
    if (key == XK_Escape or key == XK_q) return 1;
    if( key == XK_s){
      save();
    }
    if( key == XK_l){
      load();
    }
    if( key == XK_y){
    }
    if(key == XK_c){
      create = 1; selecter = 0; 
    }
    if(key == XK_v){
      create = 0; selecter = 1;  
    }
    if(key == XK_r){
      setRow(1);  
    }
    if(key == XK_t){
      setColumn(1); 
    }

    if(key == XK_a) roomX -= 50;
    if(key == XK_d) roomX += 50;
  }

  else if (e->type == KeyRelease) {
    
  
  }
  return 0;
}

//=====================================================================
//  Physics
//=====================================================================
void movement() {
  draging(savex, savey);
}

bool detectCollide (Object *obj, Object *ground) {
  //Gets (Moving Object, Static Object)
  //Reture True if Moving Object Collides with Static Object
  return (obj->getRight() > ground->getLeft() &&
      obj->getLeft()   < ground->getRight() &&
      obj->getBottom() < ground->getTop()  &&
      obj->getTop()    > ground->getBottom());
}

//=====================================================================
//  Drawing
//=====================================================================
void renderOptions(){
  if(create == 1) writeWords("Create Mode", 25, 25);
  if(selecter == 1) writeWords("Select Mode", 25, 25);
}

void renderBox(){
  glPushMatrix();
  glColor3f(0,0,0);
  glTranslatef(50, 275, 0); glBegin(GL_QUADS);
  glVertex2i(-500,-100); glVertex2i(-500, 100);
  glVertex2i(700, 100); glVertex2i(700, -100);
  glEnd();
  glPopMatrix();
}

void renderSave(){
  renderBox(); 
  writeWords("Please Check Your Terminal", 50, 275);
  writeWords("And Write Name of Saved File", 50, 250); 
  glXSwapBuffers(dpy, win);
}

void renderLoad(){
  renderBox(); 
  writeWords("Please Check Your Terminal", 50, 275);
  writeWords("And Write Name Loaded File", 50, 250); 
  glXSwapBuffers(dpy, win);
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
  renderOptions();
}

//=====================================================================
//  Storage Editor
//=====================================================================
void save(){
  std::cout << "Saving \n";
  ofstream dfs("test.ros", ios::binary); 
  dfs.write((char *)&storeIn, sizeof(storeIn));

  if(OPTIONAL_STORAGE != 1) return;
  renderSave();

  std::cout << "Save File As: ";
  string fileName;
  cin >> fileName; fileName.append(".ros");
  ofstream saveFileAs(fileName, ios::binary); 
  saveFileAs.write((char *)&storeIn, sizeof(storeIn));
}

void quickSave(){
  std::cout << "Saving \n";
  ofstream dfs("test.ros", ios::binary); 
  dfs.write((char *)&storeIn, sizeof(storeIn));
}

void load(){
      renderLoad();
      string fileName;
      std::cout << "Load in: ";  
      cin >> fileName;

      if (fileName.find(".ros") != std::string::npos) {
        std::cout << "File Exist, Will Load\n";
      }else{
        std::cout << "! Will Only take .ros files !\n";
        return;
      }
      ifstream dfs(fileName, ios::binary);
      std::cout << "what is the sizeOf(storeIn)" << sizeof(storeIn) << "\n";
      dfs.read((char *)&storeIn, sizeof(storeIn));

      for(int i = 0; i < storeIn.grounds_length; i++){
        storeIn.grounds[i].reInitSprite(); 
      } 
}

void quickLoad(){
      std::cout << "Load in \n";  
      ifstream dfs("test.ros", ios::binary);
      std::cout << "what is the sizeOf(storeIn)" << sizeof(storeIn) << "\n";
      dfs.read((char *)&storeIn, sizeof(storeIn));
}

void checkLoadFile(std::string filename){
  int size = filename.size();
  char cWords[size];
  //strcpy(cWords, words.c_str());
  //for(int i = 0; i < size; i++){
  
  //}
  
  

}
//=====================================================================
//  Window Setup
//=====================================================================
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
