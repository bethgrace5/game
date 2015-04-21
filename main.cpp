//cs335 Spring 2015 final project #include <iostream> #include <cstdlib> #include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Object.cpp"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 9001
#define GRAVITY 0.1
#define rnd()(float)rand() /(float)(RAND_MAX)

#define HERO_START_X 150
#define HERO_START_Y 700

//X Windows variables
Display *dpy; Window win; GLXContext glc;

//Hero Globals
int didJump=0;
int respawn=0;
int lives=3;

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Object *sprite);
int  check_keys (XEvent *e, Object *sprite);
void movement(Object *sprite, Object*ground);
void render(Object *sprite, Object *ground);
void moveWindow(Object *sprite);

bool collidedFromTop(Object *sprite, Object *ground);
void groundCollide(Object *sprite, Object *ground);
bool detectGroundCollide(Object *sprite, Object *ground);

int main(void){
  std::string previousPosition;
  int done=0;
  srand(time(NULL));
  initXWindows(); init_opengl();
  //declare sprite object
  Object sprite(50, 50, HERO_START_X, HERO_START_Y);
  Object ground_1( 300, 50, WINDOW_WIDTH/2, 200 );

  while(!done) { //Staring Animation
    while(XPending(dpy)) {
      //Player User Interfaces
      XEvent e; XNextEvent(dpy, &e);
      check_mouse(&e, &sprite);
      done = check_keys(&e, &sprite);
    }
    movement(&sprite, &ground_1);
    render(&sprite, &ground_1);
    moveWindow(&sprite);
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


void check_mouse(XEvent *e, Object *sprite){
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

int check_keys(XEvent *e, Object*sprite){
  //Was there input from the keyboard?
  int key = XLookupKeysym(&e->xkey, 0);
  if (e->type == KeyPress) {
    if (key == XK_Escape) return 1;
    if (key == XK_w){
      std::cout << "JUMP!! \n";
      std::cout << " velocity y: " << sprite->getVelocityY();
      std::cout << " center y: " << sprite->getCenterY();
      // TODO: disallow jumping while already in the air.
      if (didJump < 2 && sprite->getVelocityY() > -0.5){
          didJump++;
          sprite->setVelocityY(5);
      }
    }
    if (key == XK_a) {
      sprite->setVelocityX(-5);
    }
    if (key == XK_d) {
      sprite->setVelocityX(5);
    }

    // move the camera for debugging
    if (key == XK_z) {
      sprite->setCameraX( sprite->getCameraX()-10 );
    }
    if (key == XK_c) {
      sprite->setCameraX( sprite->getCameraX()+10 );
    }

    return 0;
  }
  if(e->type == KeyRelease){
    if (key == XK_a) {
      sprite->setVelocityX(0);
    }
    if (key == XK_d) {
      sprite->setVelocityX(0);
    }
  }

  return 0;
}

bool detectGroundCollide(Object *sprite, Object *ground){
  return (  sprite->getRight()  >= ground->getLeft() && 
      sprite->getLeft()   <= ground->getRight() &&
      sprite->getBottom() <= ground->getTop()  &&
      sprite->getTop()    >= ground->getBottom() 
      );
}

void groundCollide(Object *sprite, Object *ground){
  if(detectGroundCollide(sprite, ground)){
    if(!(sprite->getOldBottom() < ground->getTop()) &&
        !(sprite->getBottom() >= ground->getTop()) && (sprite->getVelocityY() < 0)){
          sprite->setVelocityY(0);
          didJump=0;
    }
    if(!(sprite->getOldTop() > ground->getBottom()) &&
        !(sprite->getTop()   <= ground->getBottom())){
          sprite->setVelocityY(-0.51); 
    }
    if(!(sprite->getOldRight() > ground->getLeft() ) &&
        !(sprite->getRight() <= ground->getLeft())){
          sprite->setVelocityX(-0.51); 
    }
    if(!(sprite->getOldLeft() < ground->getRight() ) &&
        !(sprite->getLeft() >= ground->getRight())){
      sprite->setVelocityX(0.51); 
    }
  }
  else sprite->setOldCenter();
}

void movement(Object *sprite, Object *ground){
  // Detect Collision
  groundCollide(sprite, ground); 
  // Apply Velocity, Add Gravity
  sprite->setCenter( (sprite->getCenterX() + sprite->getVelocityX()), (sprite->getCenterY() + sprite->getVelocityY()));
  sprite->setVelocityY( sprite->getVelocityY() - GRAVITY);
  // Check for Death
  if (sprite->getCenterY() < 0){
      sprite->setCenter(HERO_START_X, HERO_START_Y);
      lives--;
  }
}

void render(Object *sprite, Object *ground){
  float w, h;
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3ub(255,140,90);

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

  glColor3ub(0,140,255);

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
void moveWindow(Object *sprite) {
    double windowCenter = sprite->getWindowCenter();
    double spriteWinPos = sprite->getCenterX();
    double interval = sprite->getWindowInterval();

    //move window forward
    if (spriteWinPos > windowCenter + interval) {
        sprite->scrollWindow(5);
        sprite->setCameraX( sprite->getCameraX()-5 );
    }
    //move window backward
    else if (spriteWinPos < windowCenter - interval) {
        sprite->scrollWindow(-5);
        sprite->setCameraX( sprite->getCameraX()+5 );
    }

    // the game has just started and the sprite is not yet in
    // the center of the screen.
    if(spriteWinPos < windowCenter - interval - 5) {
        return;
    }
    // the sprite has reached the end of the level, and scrolling will
    // stop for boss battle.
    // TODO: update parameter to reflect the actual size of level.
    else if(spriteWinPos > 5000) {
        return;
    }
}
