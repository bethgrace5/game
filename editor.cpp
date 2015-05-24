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
#include <GL/glx.h>
#include <fstream>
#include "Object.cpp"
#include "bethanyA.cpp" //#include "Sprite.cpp"
#include "tedP.cpp" //#include "fastFont.h"
#include "Player.h"
#include "chadD.cpp" //Platforms/Grounds
#include "brianS.cpp" //Enemies
#include "Storage.cpp"
#include "definitions.h"


using namespace std;

typedef double Vec[3];

//X Windows variables
Display *dpy; Window win; GLXContext glc;
//------------------------------
//Game Globals
//------------------------------
Player *testHero;
Enemy *enemies[MAX_ENEMIES];
Platform *grounds[MAX_GROUNDS];
int grounds_length = 0, enemies_length;
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

void makePlatform(int x, int y); 
void setRow(int size);
void setColumn(int size);
void changeTileX();
void changeTileY();

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
void makePlatform(int x, int y) {
  cout << "Make Ground \n";
  //storeIn.grounds[storeIn.grounds_length] = new Platform();
  grounds[grounds_length] = new Platform();
  grounds[grounds_length]
    ->insert("./images/megaLevel.ppm", 10, 36);

  int width  = grounds[grounds_length]->getClipWidth();
  int height = grounds[grounds_length]->getClipHeight();

  grounds[grounds_length]->init(width, height, x, y);
  grounds[grounds_length]->setupTile();
  grounds[grounds_length]->setID(grounds_length);

  std::cout << "What is " << grounds[grounds_length]->getID() <<
    std::endl;
  grounds[grounds_length]->setIndexXY(0, 0);
  grounds[grounds_length]->setBackground(1);
  grounds_length++;
}

void setRow(int size){
  if(saveID < 0) return;
  int changeBy = 1; if(size < 0) changeBy = -1;
  int spriteWidth = grounds[saveID]->getClipWidth() * changeBy;
  int currentWidth = grounds[saveID]->getWidth();

  if(changeBy == -1 && currentWidth <= -(spriteWidth)) return;
  grounds[saveID]->setWidth(currentWidth + spriteWidth);
  grounds[saveID]->setupTile();
}

void setColumn(int size){
  if(saveID < 0) return;
  int changeBy = 1; if(size < 0) changeBy = -1;
  int spriteHeight  = grounds[saveID]->getClipHeight() * changeBy;
  int currentHeight = grounds[saveID]->getHeight();

  if(changeBy == -1 && currentHeight <= -(spriteHeight))  return;
  grounds[saveID]->setHeight(spriteHeight + currentHeight);
  grounds[saveID]->setupTile();
}

void changeTileX(){ 
  if(saveID < 0) return;
  int x = grounds[saveID]->getIndexX(); 
  int y = grounds[saveID]->getIndexY();
  grounds[saveID]->setIndexXY(x+1,y);
}

void changeTileY(){
  if(saveID < 0) return;
  int x = grounds[saveID]->getIndexX(); 
  int y = grounds[saveID]->getIndexY();
  grounds[saveID]->setIndexXY(x,y+1);
}

void deleteLastPlatform(){
  if(grounds_length <= 0) return;

  grounds[grounds_length-1] = new Platform();
  delete grounds[grounds_length-1];
  grounds_length--;
  std::cout << "What is Grounds Length " << grounds_length << std::endl;

}
void deletePlatform(){
  if(grounds_length <= 0) return;
  if(saveID < 0) return;
 
  grounds[saveID] = new Platform();
  delete grounds[saveID];

  for(int i = saveID; i < grounds_length-1; i++){
    grounds[i] = grounds[i + 1];
    grounds[i]->setID(grounds[i]->getID()-1); 
  }
  grounds[grounds_length-1] = new Platform();
  delete grounds[grounds_length-1];

  grounds_length--;
  std::cout << "What is Grounds Length " << grounds_length << std::endl;
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
        makePlatform(savex, savey);
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
    int y = roomY - WINDOW_HALF_HEIGHT;
    //int y = WINDOW_HEIGHT - e->xbutton.y;
    savex = e->xbutton.x + x; //xpast = savex;
    savey  = WINDOW_HEIGHT - e->xbutton.y + y;
  }
}

int clickObject(int x, int y){
  for(int i = 0; i < grounds_length; i++){
    if(grounds[i]->getRight()  > x &&
        grounds[i]->getLeft()   < x &&
        grounds[i]->getBottom() < y &&
        grounds[i]->getTop()    > y){
      return grounds[i]->getID(); 
    }
  }
  return -1;
}

void draging(int x, int y){
  if(holdID < 0) return;
  grounds[holdID]->setCenter(x, y);
}
//=====================================================================
//  Key Check
//=====================================================================
int check_keys (XEvent *e) {
  //handle input from the keyboard
  int key = XLookupKeysym(&e->xkey, 0);
  if (e->type == KeyPress) {
    if (key == XK_Escape or key == XK_q) return 1;
    if( key == XK_s) save(); 
    if( key == XK_l) load(); 
    if(key == XK_r) setRow(1);  
    if(key == XK_t) setColumn(1); 
    if(key == XK_y) setRow(-1);
    if(key == XK_u) setColumn(-1);
    if(key == XK_j) changeTileX();
    if(key == XK_k) changeTileY();
    if(key == XK_b) deleteLastPlatform();
    if(key == XK_n) deletePlatform();
  
    if(key == XK_a) roomX -= 50;
    if(key == XK_d) roomX += 50;
    if(key == XK_Up) roomY += 50;
    if(key == XK_Down) roomY -= 50;
    if(key == XK_Left) roomX -=50;
    if(key == XK_Right) roomX +=50;

    if(key == XK_c){
      create = 1; selecter = 0; 
    }
    if(key == XK_v){
      create = 0; selecter = 1;  
    }
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
  for (i=0;i<grounds_length;i++) {
    //if (inWindow(*(storeIn.grounds[i]))) {
    //Platform
    glPushMatrix();
    glTranslatef(- x, - y, 0);
    grounds[i]->drawRow(0,0);
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

  for(int i = 0; i < grounds_length; i++){
    storeIn.grounds[i] = *grounds[i];  
  }
  storeIn.grounds_length = grounds_length;
  std::cout << "Saving \n";
  ofstream dfs("test.ros", ios::binary); 
  dfs.write((char *)&storeIn, sizeof(storeIn));

  if(OPTIONAL_STORAGE != 1) return;
  renderSave();

  std::cout << "Save File As: ";
  string fileName;
  cin >> fileName; fileName.append(".ros");
  char charFileName[40];
  strcpy(charFileName, fileName.c_str());
  ofstream saveFileAs(charFileName, ios::binary); 

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
      char charFileName[40];
      strcpy(charFileName, fileName.c_str());

      ifstream dfs(charFileName, ios::binary);
      std::cout << "what is the sizeOf(storeIn)" << sizeof(storeIn) << "\n";
      dfs.read((char *)&storeIn, sizeof(storeIn));

      for(int i = 0; i < storeIn.grounds_length; i++){
        grounds[i] = &storeIn.grounds[i];
        grounds[i]->reInitSprite();
        grounds_length++;
      } 
}

void quickLoad(){
      std::cout << "Load in \n";  
      ifstream dfs("test.ros", ios::binary);
      std::cout << "what is the sizeOf(storeIn)" << sizeof(storeIn) << "\n";
      dfs.read((char *)&storeIn, sizeof(storeIn));
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
