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

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
#define MAX_PARTICLES 9000
#define GRAVITY 0.1
#define rnd()(float)rand() /(float)(RAND_MAX)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]

#define MAX_BACKGROUND_BITS 1000
#define HERO_START_X 330
#define HERO_START_Y 420

extern "C" {
#include "fonts.h"
}

using namespace std;

typedef double Vec[3];

struct bgBit {
    Vec pos;
    Vec lastpos;
    Vec vel;
    struct bgBit *next;
    struct bgBit *prev;
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
int isWalking=0;
int isFalling=0;
int isDying=0;
int isJumping=0;
double s_right, s_left, s_top, s_bottom;
timeval seqStart, seqEnd;

//Game Globals
bgBit *bitHead = NULL;
Object *grounds[32] = {NULL};
int bg, gr;
int roomX=WINDOW_WIDTH/2;
int roomY=WINDOW_HEIGHT/2;
int fail=0;
int interval=120;
double g_left, g_right, g_top, g_bottom;

//Images and Textures
Ppmimage *heroImage=NULL;
GLuint heroTexture;

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Object *sprite);
int  check_keys (XEvent *e, Object *sprite);
void movement(Object *sprite);
void render(Object *sprite);
void moveWindow(Object *sprite);
void renderBackground(void);
void cleanup_background(void);

void groundCollide(Object *sprite, Object *ground);
bool detectCollide(Object *sprite, Object *ground);

int main(void){
    string previousPosition;
    timeval end, start;
    gettimeofday(&start, NULL);
    int done=0;
    srand(time(NULL));
    initXWindows(); init_opengl();
    //declare sprite object
    Object sprite(46, 48, HERO_START_X, HERO_START_Y);
    sprite.setTop(44);
    sprite.setBottom(-44);
    sprite.setLeft(-26);
    sprite.setRight(26);
    Object ground_0(10, 1000, -10, 600);
    Object ground_1(400, 10, 400, 80);
    Object ground_2(200, 10, 900, 200);
    Object ground_3(150, 10, 1200, 360);
    Object ground_4(250, 10, 1450, 80);
    Object ground_5(450, 10, 2500, 80);
    Object ground_6(350, 10, 2500, 360);
    Object ground_7(250, 10, 2800, 480);
    Object ground_8(450, 10, 3500, 80);
    Object ground_9(450, 10, 4000, 200);
    Object ground_10(450, 10, 4500, 80);
    Object ground_11(450, 10, 5500, 80);
    Object ground_12(450, 10, 6500, 80);
    Object ground_13(450, 10, 7500, 80);
    Object ground_14(450, 10, 8500, 80); 
    
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

    gr=15;
    while(!done) { //Staring Animation
        while(XPending(dpy)) {
            //Player User Interfaces
            XEvent e; XNextEvent(dpy, &e);
            check_mouse(&e, &sprite);
            done = check_keys(&e, &sprite);
        }
        movement(&sprite);
        render(&sprite);
        gettimeofday(&end, NULL);
        if (diff_ms(end, start) > 1200)
            moveWindow(&sprite);
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
        //hose(&sprite);
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
    for (int i=0; i<img->width * img->height * 3; i+=3) {
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
        if (key == XK_w || key == XK_Up){
            //Jump
            if (didJump < 2 && sprite->getVelocityY() > -0.5){
                didJump++;
                sprite->setVelocityY(5);
            }
        }
        if (key == XK_a || key == XK_Left) {
            sprite->setVelocityX(-5);
        }
        if (key == XK_d || key == XK_Right) {
            sprite->setVelocityX(5);
        }
        if (key == XK_space) {
            //shoot bullet
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
        if (key == XK_a || key == XK_Left) {
            sprite->setVelocityX(0);
        }
        if (key == XK_d || key == XK_Right) {
            sprite->setVelocityX(0);
        }
    }

    return 0;
}

bool detectCollide(Object *sprite, Object *ground){
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (  sprite->getRight()  > ground->getLeft() && 
            sprite->getLeft()   < ground->getRight() &&
            sprite->getBottom() < ground->getTop()  &&
            sprite->getTop()    > ground->getBottom() 
           );
}

void groundCollide(Object *sprite, Object *ground){
    //(Moving Object, Static Object)
    //Detects Which boundaries the Moving Object is around the Static Object
    //top,down,left,right
    if(detectCollide(sprite, ground)){
        s_right=sprite->getRight();
        s_left=sprite->getLeft();
        s_top=sprite->getTop();
        s_bottom=sprite->getBottom();
        g_right=ground->getRight();
        g_bottom=ground->getBottom();
        g_top=ground->getTop();
        g_left=ground->getLeft();
        //If moving object is on top of the static object
        if(!(sprite->getOldBottom() < g_top) &&
                !(s_bottom >= g_top) && (sprite->getVelocityY() < 0)){
            sprite->setVelocityY(0);
            sprite->setCenter(sprite->getCenterX(), 
                    g_top+(sprite->getCenterY()-s_bottom)
                    );
            didJump=0;
        }
        //If moving object is at the bottom of static object
        if(!(sprite->getOldTop() > g_bottom) &&
                !(s_top <= g_bottom)){
            sprite->setVelocityY(-0.51); 
            sprite->setCenter(sprite->getCenterX(),
                    g_bottom-(s_top-sprite->getCenterY())
                    );
        }
        //If moving object is at the l-eft side of static object
        if(!(sprite->getOldRight() > g_left ) &&
                !(s_right <= g_left)){
            sprite->setVelocityX(-0.51); 
            sprite->setCenter(g_left-(s_right-sprite->getCenterX()),
                    sprite->getCenterY()
                    );
        }
        //If moving object is at the right side of static object
        if(!(sprite->getOldLeft() < g_right ) &&
                !(s_left >= g_right)){
            sprite->setVelocityX(0.51); 
            sprite->setCenter(g_right+(sprite->getCenterX()-s_left),
                    sprite->getCenterY()
                    );
        }
    }	
}

void movement(Object *sprite){
    Object *ground;
    for (int i=0; i<gr; i++){
        ground = grounds[i];
        // Detect Collision
        groundCollide(sprite, ground); 
    }
    sprite->setOldCenter();
    // Apply Velocity, Add Gravity
    sprite->setCenter( (sprite->getCenterX() + sprite->getVelocityX()), (sprite->getCenterY() + sprite->getVelocityY()));
    // Cycle through index sequences
    if (sprite->getVelocityY() < -1){
        isFalling=1;
        isJumping=isWalking=0;
        if (didJump<1)
            sprite->setIndex(7);
    } else if (sprite->getVelocityY() > 1) {
        isJumping=1;
        isWalking=isFalling=0;
        if (didJump>1)
            sprite->setIndex(0);
        else
            sprite->setIndex(1);
    } else {
        isFalling=0;
        if (sprite->getVelocityX() < -1 or 
                sprite->getVelocityX() > 1){
            if (!isWalking){
                isWalking=1;
                sprite->setIndex(0);
                gettimeofday(&seqStart, NULL);
            }
            gettimeofday(&seqEnd, NULL);
            if ((diff_ms(seqEnd, seqStart)) > 80){
                sprite->setIndex(((sprite->getIndex()+1)%6));
                gettimeofday(&seqStart, NULL);
            }
            isFalling=isJumping=0;
        } else {
            isWalking=0;
            if (!isJumping)
                sprite->setIndex(6);
        }
    }
    if (!isWalking and !isFalling and !isJumping)
        sprite->setVelocityX(0);
    // Check for Death
    if (sprite->getCenterY() < 0){
        sprite->setCenter(HERO_START_X, HERO_START_Y);
        lives--;
        fail=100;
        sprite->setVelocityX(0);
    }
    sprite->setVelocityY( sprite->getVelocityY() - GRAVITY);
}

void render(Object *sprite){
    float w, h, tl_sz;
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw Background Falling Bits
    renderBackground();

    glColor3ub(65,155,225);
    Object *ground;
    for (int i=0;i<gr;i++){
        ground = grounds[i];
        //Ground
        glPushMatrix();
        glTranslatef(
                ground->getCenterX() + sprite->getCameraX(),
                ground->getCenterY() + sprite->getCameraY(), 
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


    // Draw Hero Sprite
    glPushMatrix();
    glTranslatef(
            sprite->getCenterX() + sprite->getCameraX(),
            sprite->getCenterY() + sprite->getCameraY(),
            0);
    w = sprite->getWidth();
    h = sprite->getHeight();
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    tl_sz = 0.076923077;
    if ((sprite->getVelocityX() < 0.0) or (sprite->getOldCenterX()>sprite->getCenterX())){
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz), 0.0f); glVertex2i(w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz), 1.0f); glVertex2i(w,-w);
    } else {
        glTexCoord2f(sprite->getIndex()*tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f(sprite->getIndex()*tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(w,-w);
    }
    glEnd(); glPopMatrix();
    glDisable(GL_ALPHA_TEST);

    // font printing
    Rect r0, r1;
    r0.bot = WINDOW_HEIGHT - 32;
    r0.left = r0.center = 32;
    ggprint12(&r0, 16, 0x0033aaff, "Lives ");
    r1.bot = WINDOW_HEIGHT/2;
    r1.left = r1.center = WINDOW_WIDTH/2;
    if (fail>0){
        ggprint16(&r1, fail/2, 0x00ff0000, "FAIL");
        fail--;
    }

}
void moveWindow(Object *sprite) {
    double spriteWinPosX = sprite->getCenterX();
    double spriteWinPosY = sprite->getCenterY();

    //move window forward
    if (spriteWinPosX > roomX + interval) {
        sprite->setCameraX( sprite->getCameraX()-5 );
        roomX+=5;
    }
    //move window backward (fast move if checkpoint far away)
    else if ((spriteWinPosX < roomX - interval) && roomX>(WINDOW_WIDTH/2)) {
        sprite->setCameraX( sprite->getCameraX()+5 );
        roomX-=5;
            if (spriteWinPosX < (roomX - interval - 400)){
                roomX-=20;
                sprite->setCameraX(sprite->getCameraX()+20);
            }
            if (spriteWinPosX < (roomX - interval - 800)){
                roomX-=50;
                sprite->setCameraX(sprite->getCameraX()+50);
            }
    }
    //move window up
    if (spriteWinPosY > roomY + interval) {
        sprite->setCameraY( sprite->getCameraY()-5 );
        roomY+=5;
    }
    //move window down
    else if ((spriteWinPosY < roomY - interval) && roomY>(WINDOW_HEIGHT/2)) {
        sprite->setCameraY( sprite->getCameraY()+5 );
        roomY-=5;
    }

    // TODO: update parameter to reflect the actual size of level.
    else if(spriteWinPosX > 5000) {
        return;
    }
}
void renderBackground(){
    if (bg < MAX_BACKGROUND_BITS){
        // Create bit
        bgBit *bit = new bgBit;
        if (bit == NULL){
            exit(EXIT_FAILURE);
        }
        bit->pos[0] = (rnd() * ((float)WINDOW_WIDTH+3000)) +
            (roomX-(WINDOW_WIDTH/2)) - 1000;
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
        Rect r0;
        r0.bot = bit->pos[1];
        r0.left = r0.center = (bit->pos[0]-((roomX-(WINDOW_WIDTH/2))*bit->pos[2]));
        int i, j = bit->pos[2];
        if (bit->pos[1]>(WINDOW_HEIGHT*0.7)){
            i=255;
        } else {
            i = (bit->pos[1]-10)/(WINDOW_HEIGHT/255);
            if (i<1)
                i=0;
        }
        if (j>=1){
            ggprint12(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
        } else if (j>0.9) {
            ggprint10(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
        } else {
            ggprint08(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
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
