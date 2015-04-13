//cs335 Spring 2015 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//Assignment is to modify this program.
//You will follow along with your instructor.
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 9001
#define GRAVITY 0.1

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

struct Game {
  bool bubbler;
  Shape ground;
  Shape box; Shape circle;
  Particle *particle;
  int n;

  ~Game(){ delete [] particle;}
  Game(){
    particle = new Particle[MAX_PARTICLES];
    n = 0;
    //Declare a box shape
    box.width  = 50;
    box.height = 50;
    box.center.x = 170;
    box.center.y = 150;
    box.center.z = 0;
  
    //Circle
    circle.radius = 100  ; circle.center.x = 650;
    circle.center.y = 150; circle.center.z = 0;

    //Object to land on
    ground.width = 500    ; ground.height = 50;
    ground.center.x = 450; ground.center.y = 200; ground.center.z = 0;
  }
};

//Function prototypes
void initXWindows(void)   ; void init_opengl(void);
void cleanupXWindows(void);

void check_mouse(XEvent *e, Game *game);
int  check_keys (XEvent *e, Game *game);

void movement(Game *game); void render(Game *game);

int main(void){
  int done=0; srand(time(NULL));
  initXWindows(); init_opengl();
  //declare game object
  Game game; game.n=0;

  while(!done) { //Staring Animation

    while(XPending(dpy)) { 
      //Player User Interfaces
      XEvent e; XNextEvent(dpy, &e);
      check_mouse(&e, &game);
      done = check_keys(&e, &game);
    }
    movement(&game); render(&game);
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
    //hose(&game);
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

#define rnd()(float)rand() /(float)(RAND_MAX)

int hoseOn = 0; int still = 1; int xpast, ypast;
void check_mouse(XEvent *e, Game *game){
  static int savex = 0, savey = 0;
  static int n = 0;
  if (e->type == ButtonRelease) {
    xpast = savex; ypast = savey;
    still = 1; return;
  }
  if (e->type == ButtonPress) {
    if (e->xbutton.button==1) { //Left button was pressed
      int y = WINDOW_HEIGHT - e->xbutton.y;
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

int keyChecker = 0;
int check_keys(XEvent *e, Game *game){
  //Was there input from the keyboard?
  int key = XLookupKeysym(&e->xkey, 0);
  if (e->type == KeyPress) {
    //int key = XLookupKeysym(&e->xkey, 0);
    if (key == XK_Escape) {
      return 1;
    }
    if(key == XK_w){
      std::cout << "JUMP!! \n";
      game->box.velocity.y = 10;
    }
    if(key == XK_a){
      game->box.velocity.x = -5; 
    }
    if(key == XK_d){
      game->box.velocity.x = 5;
    }
    return 0;
  }
  if(e->type == KeyRelease){
    if(key == XK_a){
        game->box.velocity.x = 0;
      }
      if(key == XK_d){
        game->box.velocity.x = 0;
      }
  }

  return 0;
}


void movement(Game *game){

  //Collusion
  float boxLeft  = game->box.center.x - game->box.width;
  float boxRight = game->box.center.x + game->box.width;
  float boxTop   = game->box.center.y + game->box.height;
  float boxDown  = game->box.center.y - game->box.height;

  float groundLeft  = game->ground.center.x - game->ground.width;
  float groundRight = game->ground.center.x + game->ground.width;
  float groundTop   = game->ground.center.y + game->ground.height;
  float groundDown  = game->ground.center.y - game->ground.height;
  int collideX = 0; int collideY = 0;
  
  float dx = boxRight - boxLeft;
  float dy = 0;
  int collide = 0;
  if(  boxRight >= groundLeft
    && boxLeft  <= groundRight
    && boxDown  <=  groundTop
    && boxTop   >=  groundDown){
    if(game->box.velocity.y < 0) game->box.velocity.y = 0.0;
    //if(game->box.velocity.y > 0 && boxTop > groundDown) game->box.velocity.y = -5.0;
  }

  //if(boxDown  < groundTop)  game->box.velocity.y = 0.5;
  //if(boxTop   > groundDown) game->box.velocity.y = -0.5;
  //if(boxRight < groundLeft) collide = 1;
  //if(boxLeft  > groundRight) collide = 1;
  /*if (groundLeft > boxRight || groundRight < boxLeft ||
       groundTop > boxDown   || groundDown < boxTop);
  else std::cout << "COLLLIDE! \n";*/

/*
  float boxX = game->box.center.x;
  float boxY = game->box.center.y; 
  float groundX = game->ground.center.x;
  float groundY = game->ground.center.y;
  if(     boxX - game->box.width < groundX + game->ground.width
      &&  boxX - game->box.width > groundX - game->ground.width
      &&  game->box.velocity.x < 0){

    game->box.velocity.x = 0; 
  }
  if(     boxX + game->box.width > groundX - game->ground.width
      &&  boxX + game->box.width < groundX + game->ground.width
      &&  game->box.velocity.x > 0)
  {

    game->box.velocity.x = 0; 
  }*/

  game->box.center.y += game->box.velocity.y;
  game->box.center.x += game->box.velocity.x;

  if(game->box.center.y - game->box.height/2 > 0){ 
    if(collideY != 1) game->box.velocity.y -= GRAVITY;
  }
  else game->box.velocity.y = 0;

}

void render(Game *game){
  float w, h;
  glClear(GL_COLOR_BUFFER_BIT);
  //Draw shapes...

  //draw box
  Shape *s; glColor3ub(90,140,90);

  //Player Controler Box
  s = &game->box;
  glPushMatrix();
  glTranslatef(s->center.x, s->center.y, s->center.z);
  w = s->width; 
  h = s->height;
  glBegin(GL_QUADS);
  glVertex2i(-w,-h);
  glVertex2i(-w, h);
  glVertex2i( w, h);
  glVertex2i( w,-h);
  glEnd(); glPopMatrix();
  
  //Static Objects// 
  s = &game->ground;
  glPushMatrix();
  glTranslatef(s->center.x, s->center.y, s->center.z);
  w = s->width; 
  h = s->height;
  glBegin(GL_QUADS);
  glVertex2i(-w,-h);
  glVertex2i(-w, h);
  glVertex2i( w, h);
  glVertex2i( w,-h);
  glEnd(); glPopMatrix(); 
}




