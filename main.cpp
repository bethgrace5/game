//cs335 Spring 2015 final project #include <iostream> #include <cstdlib> #include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "Object.cpp"
#include "ppm.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define WINDOW_HALF_WIDTH  WINDOW_WIDTH/2
#define WINDOW_HALF_HEIGHT WINDOW_HEIGHT/2
#define LEVEL_WIDTH 10000
#define MAX_HEIGHT 1000
#define GRAVITY 0.2
#define rnd()(float)rand() /(float)(RAND_MAX)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]

#define MAX_BACKGROUND_BITS 6000
#define HERO_START_X 150
#define HERO_START_Y 350

extern "C" {
#include "fonts.h"
}

using namespace std;

typedef double Vec[3];

// background bits
struct bgBit {
    Vec pos;
    Vec lastpos;
    Vec vel;
    struct bgBit *next;
    struct bgBit *prev;
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

int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec))/1000;
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
double h_right, h_left, h_top, h_bottom;
timeval seqStart, seqEnd;

//Game Globals
bgBit *bitHead = NULL;
Object *grounds[32] = {NULL};
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

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e);
int  check_keys (XEvent *e, Object *hero);
void movement(Object *hero);
void render(Object *hero);
void renderMenu();
void moveWindow(Object *hero);
void renderBackground(void);
void cleanup_background(void);
Object createAI( int w, int h, Object *ground);

void groundCollide(Object *hero, Object *ground);
bool detectCollide(Object *hero, Object *ground);

bool inWindow(Object &obj){
    return ((obj.getLeft() < (roomX+(WINDOW_HALF_WIDTH)) and
             obj.getLeft() > (roomX-(WINDOW_HALF_WIDTH))) or 
            (obj.getRight() > (roomX-(WINDOW_HALF_WIDTH)) and
             obj.getRight() < (roomX+(WINDOW_HALF_WIDTH))));
}

int main(void){
    //string previousPosition;
    //srand(time(NULL));
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
    Object ground_6(350, 10, 2500, 360);
    Object ground_7(250, 10, 2800, 480);
    Object ground_8(440, 10, 3500, 80);
    Object ground_9(440, 10, 4000, 200);
    Object ground_10(440, 10, 4500, 80);
    Object ground_11(440, 10, 5500, 80);
    Object ground_12(440, 10, 6500, 80);
    Object ground_13(440, 10, 7500, 80);
    Object ground_14(440, 10, 8500, 80); 

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
    grounds_length=15;

    //setup enemies
    Object enemy_0 = createAI(20, 48, &ground_2);
    Object enemy_1 = createAI(20, 48, &ground_3);

    enemies[0] = &enemy_0;
    enemies[1] = &enemy_1;
    enemies_length=2;

    level=1;

    while(!quit) { //Staring Animation
        while(XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e);
            quit = check_keys(&e, &hero);
        }
        if (level>0){
            movement(&hero);
            render(&hero);
            moveWindow(&hero);
        }
        else{
            renderMenu();
        }
        glXSwapBuffers(dpy, win);
    }
    cleanupXWindows(); return 0;
}

void set_title(void){ //Set the window title bar.
    XMapWindow(dpy, win); XStoreName(dpy, win, "Box Movement");
}

void cleanupXWindows(void) { //do not change
    cleanup_background();
    XDestroyWindow(dpy, win); XCloseDisplay(dpy);
}

void initXWindows(void) { //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        cout << "\n\tcannot connect to X server\n" << endl;
        //hose(&hero);
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if(vi == NULL) {
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

unsigned char *buildAlphaData(Ppmimage *img){
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

void init_opengl(void){
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
    initialize_fonts();
    //Load images into ppm structure.
    heroImage = ppm6GetImage("./images/hero.ppm");
    //Create texture elements
    glGenTextures(1, &heroTexture);
    int w = heroImage->width;
    int h = heroImage->height;
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //must build a new set of data...
    unsigned char *silhouetteData = buildAlphaData(heroImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
    delete [] silhouetteData;
}

void check_mouse(XEvent *e){
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

int check_keys(XEvent *e, Object*hero){
    //Was there input from the keyboard?
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape) return 1;
        if ((key == XK_w || key == XK_Up) && !isDying){
            //Jump
            if (didJump < 2 && hero->getVelocityY() > -0.5){
                didJump++;
                hero->setVelocityY(8);
            }
        }
        if ((key == XK_a || key == XK_Left) && !isDying) {
            hero->setVelocityX(-6);
        }
        if ((key == XK_d || key == XK_Right) && !isDying) {
            hero->setVelocityX(6);
        }
        if (key == XK_space) {
            life-=1000;
        }

        return 0;
    }
    if(e->type == KeyRelease){
        if (key == XK_a || key == XK_Left) {
            hero->setVelocityX(0);
        }
        if (key == XK_d || key == XK_Right) {
            hero->setVelocityX(0);
        }
    }

    return 0;
}

bool detectCollide(Object *hero, Object *ground){
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (  hero->getRight()  > ground->getLeft() &&
            hero->getLeft()   < ground->getRight() &&
            hero->getBottom() < ground->getTop()  &&
            hero->getTop()    > ground->getBottom()
           );
}

void groundCollide(Object *hero, Object *ground){
    //(Moving Object, Static Object)
    //Detects Which boundaries the Moving Object is around the Static Object
    //top,down,left,right
    if(detectCollide(hero, ground)){
        h_right=hero->getRight();
        h_left=hero->getLeft();
        h_top=hero->getTop();
        h_bottom=hero->getBottom();
        g_right=ground->getRight();
        g_bottom=ground->getBottom();
        g_top=ground->getTop();
        g_left=ground->getLeft();
        //If moving object is on top of the static object
        if(!(hero->getOldBottom() < g_top) &&
                !(h_bottom >= g_top) && (hero->getVelocityY() < 0)){
            hero->setVelocityY(0);
            hero->setCenter(hero->getCenterX(),
                    g_top+(hero->getCenterY()-h_bottom)
                    );
            isFalling=isJumping=didJump=0;
        }
        //If moving object is at the bottom of static object
        if(!(hero->getOldTop() > g_bottom) &&
                !(h_top <= g_bottom)){
            hero->setVelocityY(-0.51);
            hero->setCenter(hero->getCenterX(),
                    g_bottom-(h_top-hero->getCenterY())
                    );
        }
        //If moving object is at the l-eft side of static object
        if(!(hero->getOldRight() > g_left ) &&
                !(h_right <= g_left)){
            hero->setVelocityX(-0.51);
            hero->setCenter(g_left-(h_right-hero->getCenterX()),
                    hero->getCenterY()
                    );
        }
        //If moving object is at the right side of static object
        if(!(hero->getOldLeft() < g_right ) &&
                !(h_left >= g_right)){
            hero->setVelocityX(0.51);
            hero->setCenter(g_right+(hero->getCenterX()-h_left),
                    hero->getCenterY()
                    );
        }
    }
}

void movement(Object *hero){
    Object *ground;
    for (i=0; i<grounds_length; i++){
        ground = grounds[i];
        // Detect Collision
        groundCollide(hero, ground);
    }
    hero->setOldCenter();
    // Apply Velocity, Add Gravity
    hero->setCenter( (hero->getCenterX() + hero->getVelocityX()), (hero->getCenterY() + hero->getVelocityY()));
    // Cycle through index sequences
    if (life<=0){
        hero->setVelocityX(0);
        isWalking=0;
        if (!isDying){
            isDying=1;
            hero->setIndex(7);
            gettimeofday(&seqStart, NULL);
            fail=100;
        }
        else{
            gettimeofday(&seqEnd, NULL);
            if (((diff_ms(seqEnd, seqStart)) > 100) && (!isFalling && !isJumping)){
                if ((hero->getIndex()<12)){
                    hero->setIndex(hero->getIndex()+1);
                    gettimeofday(&seqStart, NULL);
                }
                else{
                    if (((diff_ms(seqEnd, seqStart)) > 500)){
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
    if ((hero->getVelocityY() < -1) && !isDying){ // Falling
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
        if (hero->getVelocityX() < -1 or
                hero->getVelocityX() > 1){
            if (!isWalking && !isJumping && !isFalling){
                isWalking=1;
                hero->setIndex(0);
                gettimeofday(&seqStart, NULL);
            }
            gettimeofday(&seqEnd, NULL);
            if (isWalking && ((diff_ms(seqEnd, seqStart)) > 80)){
                hero->setIndex(((hero->getIndex()+1)%6));
                gettimeofday(&seqStart, NULL);
                isFalling=isJumping=0;
            }
        } else {
            isWalking=0;
            if (!isJumping)
                hero->setIndex(6);
        }
    }
    if (!isWalking and !isFalling and !isJumping)
        hero->setVelocityX(0);
    // Check for Death
    if (hero->getCenterY() < 0){
        hero->setCenter(HERO_START_X, HERO_START_Y);
        lives--;
        life=fail=100;
        hero->setVelocityX(0);
        isDying=0;
    }
    hero->setVelocityY( hero->getVelocityY() - GRAVITY);
}

void renderMenu(){

}

void render(Object *hero){
    float w, h, tl_sz, x, y;
    x = roomX - WINDOW_HALF_WIDTH;
    y = roomY - WINDOW_HALF_HEIGHT;
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw Background Falling Bits
    renderBackground();

    glColor3ub(65,155,225);
    // render grounds
    Object *ground;
    for (i=0;i<grounds_length;i++){
        ground = grounds[i];
        if (inWindow(*ground)){
            //Ground
            glPushMatrix();
            glTranslatef(
                    ground->getCenterX() - x,
                    ground->getCenterY() - y,
                    0);
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
    // render enemies
    glColor3ub(100,0,0);
    Object *enemy;
    for (int i=0;i<enemies_length;i++){
        enemy = enemies[i];
        if (inWindow(*enemy)){
            //Enemy
            glPushMatrix();
            glTranslatef(
                    enemy->getCenterX() - x,
                    enemy->getCenterY() - y,
                    0);
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
    //Non-Collision Object
    /*
       glPushMatrix();
       glTranslatef(s->center.x + 600 + hero->camera.x , s->center.y, s->center.z);
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
    // the hero should be 'left' at the beginning of the level,
    // 'mid' throughout the level, and 'right' at the end of the level.
    // retuns the position of the hero as left, mid, or right.


    // Draw Hero Sprite
    glPushMatrix();
    glTranslatef(
            hero->getCenterX() - x,
            hero->getCenterY() - y,
            0);
    w = hero->getWidth();
    h = hero->getHeight();
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    tl_sz = 0.076923077;
    if ((hero->getVelocityX() < 0.0) or (hero->getOldCenterX()>hero->getCenterX())){
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero->getIndex()*tl_sz), 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero->getIndex()*tl_sz), 1.0f); glVertex2i(w,-w);
    } else {
        glTexCoord2f(hero->getIndex()*tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f(hero->getIndex()*tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(w,-w);
    }
    glEnd(); glPopMatrix();
    glDisable(GL_ALPHA_TEST);

    // font printing
    Rect r0, r1;
    r0.bot = WINDOW_HEIGHT - 32;
    r0.left = r0.center = 32;
    ggprint12(&r0, 16, 0x0033aaff, "Lives ");
    r1.bot = WINDOW_HALF_HEIGHT;
    r1.left = r1.center = WINDOW_HALF_WIDTH;
    if (fail>0){
        ggprint16(&r1, fail/2, 0x00ff0000, "FAIL");
        fail--;
    }

}
void moveWindow(Object *hero) {
    double heroWinPosX = hero->getCenterX();
    double heroWinPosY = hero->getCenterY();

    //move window forward
    if ((heroWinPosX > roomX + interval) && ((roomX+WINDOW_HALF_WIDTH)<LEVEL_WIDTH)) {
        roomX+=6;
    }
    //move window backward (fast move if hero is far away)
    else if ((heroWinPosX < roomX - interval) && roomX>WINDOW_HALF_WIDTH) {
        roomX-=6;
            if (heroWinPosX < (roomX - interval - 400)){
                roomX-=20;
            }
            if (heroWinPosX < (roomX - interval - 800)){
                roomX-=50;
            }
    }
    //move window up
    if (heroWinPosY > roomY + interval) {
        roomY+=6;
    }
    //move window down
    else if ((heroWinPosY < roomY - interval) && roomY>(WINDOW_HALF_HEIGHT)) {
        i = hero->getVelocityY();
        if (i>-6)
            i=-6;
        roomY+=i;
    }

    // TODO: update parameter to reflect the actual size of level.
    else if(heroWinPosX > 5000) {
        return;
    }
}
void renderBackground(){
    if (bg < 1){
        for (i=0;i<=(MAX_BACKGROUND_BITS/3);i++){
            bgBit *bit = new bgBit;
            if (bit == NULL){
                exit(EXIT_FAILURE);
            }
            bit->pos[0] = (rnd() * (LEVEL_WIDTH));
            bit->pos[1] = (rnd() * (float)MAX_HEIGHT);
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
    }
    if (bg < MAX_BACKGROUND_BITS){
        // Create bit
        //for (i=0;i<=1;i++){
            bgBit *bit = new bgBit;
            if (bit == NULL){
                exit(EXIT_FAILURE);
            }
            bit->pos[0] = (rnd() * (LEVEL_WIDTH));
            bit->pos[1] = (rnd() * 100) + (float)MAX_HEIGHT;
            bit->pos[2] = 0.8 + (rnd() * 0.4);
            bit->vel[0] = 0.0f;
            bit->vel[1] = -0.8f;
            bit->vel[2] = (rnd());
            bit->next = bitHead;
            if (bitHead != NULL)
                bitHead->prev = bit;
            bitHead = bit;
            bg++;
        //}
    }
    // Reset pointer to beginning to render all bits
    bgBit *bit = bitHead;
    while (bit){
        VecCopy(bit->pos, bit->lastpos);
        if (bit->pos[1] > 0){
            bit->pos[1] += bit->vel[1];

        }
        else{
            bgBit *savebit = bit->next;
            if (bit->prev == NULL){
                if (bit->next == NULL){
                    bitHead = NULL;
                } else {
                    bit->next->prev = NULL;
                    bitHead = bit->next;
                }
            } else {
                if (bit->next == NULL){
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
        j = (bit->pos[0]-((roomX-(WINDOW_HALF_WIDTH))*bit->pos[2]));
        if ((j+100>(0)) && (j-100<(WINDOW_WIDTH))){
            Rect r0;
            r0.bot = (bit->pos[1]-((roomY-(WINDOW_HALF_HEIGHT))*bit->pos[2]));
            r0.left = r0.center = j;
            j = bit->pos[2];
            //if (bit->pos[1]>(WINDOW_HEIGHT*0.7)){
              //  i=255;
            //} else {
                i = (bit->pos[1])/(WINDOW_HEIGHT/255);
                if (i<0)
                  i=0;
                if (i>255)
                    i=255;
            //}
            i=(i*65536+256*i+i);
            if (j>=1){
                ggprint12(&r0, 16, i, (bit->vel[2]>0.5?"1":"0") );
            } else if (j>0.9) {
                ggprint10(&r0, 16, i, (bit->vel[2]>0.5?"1":"0") );
            } else {
                ggprint08(&r0, 16, i, (bit->vel[2]>0.5?"1":"0") );
            }
        }
        bit = bit->next;
    }
    glLineWidth(1);
}

void cleanup_background(void){
    bgBit *s;
    while (bitHead) {
        s = bitHead->next;
        delete bitHead;
        bitHead = s;
    }
    bitHead = NULL;
}

Object createAI( int w, int h, Object *ground) {
    Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
    //cout << glGetIntegerv(GL_VIEWPORT);
    return newEnemy;

}

