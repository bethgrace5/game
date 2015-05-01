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
#include "ppm.h"

#include "GameState.h"
#include "GameEngine.h"
#include "PlayState.h"
#include "fonts.h"
#include "Object.h"
//#include "menustate.h"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

#define MAX_BACKGROUND_BITS 1000
#define HERO_START_X 150
#define HERO_START_Y 350

#define GRAVITY 0.1
#define rnd()(float)rand() /(float)(RAND_MAX)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
using namespace std;
//XEvent *e; 

PlayState PlayState::m_PlayState;
typedef double Vec[3];
// background bits
struct bgBit {
    Vec pos;
    Vec lastpos;
    Vec vel;
    struct bgBit *next;
    struct bgBit *prev;
};

//Game Globals
bgBit *bitHead = NULL;
Object *grounds[32] = {NULL};
Object *enemies[32] = {NULL};
int bg, grounds_length, enemies_length;
int roomX=WINDOW_WIDTH/2;
int roomY=WINDOW_HEIGHT/2;
int fail=0;
int interval=120;
double g_left, g_right, g_top, g_bottom;

//Hero Globals
int didJump=0;
int lives=3;
int life=100;
int isWalking=0;
int isFalling=0;
int isDying=0;
int isJumping=0;
double s_right, s_left, s_top, s_bottom;
timeval seqStart, seqEnd;


//Images and Textures
Ppmimage *heroImage=NULL;
GLuint heroTexture;

Object hero(46, 48, HERO_START_X + 50, HERO_START_Y + 50);
timeval end, start;

void PlayState::Init() {
    gettimeofday(&start, NULL);
    //int done=0;
    srand(time(NULL));

    // declare all ground objects
    // declare all sprite objects
    //declare hero object
    hero.setTop(44);
    hero.setBottom(-44);
    hero.setLeft(-26);
    hero.setRight(26);

    Object ground_0(10, 1000, -10, 600);

    grounds[0] = &ground_0;
    grounds_length=1;

    //setup enemies
    //Object enemy_0 = createAI(20, 48, &ground_2);
    //Object enemy_1 = createAI(20, 48, &ground_3);

    //enemies[0] = &enemy_0;
    //enemies[1] = &enemy_1;
    enemies_length=0;

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
//}
}

void PlayState::Cleanup() {
    //clean background
    bgBit *s;
    while (bitHead) {
        s = bitHead->next;
        delete bitHead;
        bitHead = s;
    }
    bitHead = NULL;

}


void PlayState::Pause() {
}

void PlayState::Resume() {
}

void PlayState::HandleEvents(GameEngine* game) {
    int done = 0;
        if(XPending(game->dpy)) {
            while(!done) {
                cout << "xpending";
                //Player User Interfaces
                XEvent e; XNextEvent(game->dpy, &e);
                check_mouse(&e);
                done = check_keys(&e, &hero, game);
            }
        glXSwapBuffers(game->dpy, game->win);
    }
}

int PlayState::check_mouse(XEvent *e) {

    static int savex = 0, savey = 0;
    //static int n = 0;
    if (e->type == ButtonRelease) { return 0;}
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) { //Left button was pressed
            //int y = WINDOW_HEIGHT - e->xbutton.y;
            return 0;
        }
        if (e->xbutton.button==3) { //Right button was pressed
            return 0;
        }
    }

    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        savex = e->xbutton.x; //xpast = savex;
        savey = e->xbutton.y; //ypast = savey;
    }
    return 1;
}

    //check_keys
int PlayState::check_keys(XEvent *e, Object *hero, GameEngine *game){
    //Was there input from the keyboard?
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        if (key == XK_Escape) exit(0);
        if ((key == XK_w || key == XK_Up) && !isDying){
            //Jump
            if (didJump < 2 && hero->getVelocityY() > -0.5){
                didJump++;
                hero->setVelocityY(5);
            }
        }
        if ((key == XK_a || key == XK_Left) && !isDying) {
            hero->setVelocityX(-5);
            cout << "hit a"<<endl;
        }
        if ((key == XK_d || key == XK_Right) && !isDying) {
            hero->setVelocityX(5);
        }
        if (key == XK_space) {
            life-=1000;
        }
        if (key == XK_q) {
            game->Quit();
        }
        if (key == XK_i) {
            game->ChangeState( PlayState::Instance() );
            //game->ChangeState();
            //game->Quit();
        }

    }
    if(e->type == KeyRelease){
        if (key == XK_a || key == XK_Left) {
            hero->setVelocityX(0);
        }
        if (key == XK_d || key == XK_Right) {
            hero->setVelocityX(0);
        }
    }

    return 1;
//}
}

void PlayState::Update(GameEngine* game) {
    //int done = 0;
    //while(!done) { //Staring Animation
            //done = check_keys(&e, &hero);
        //}
        movement(&hero);
        //render(&hero);
        //gettimeofday(&end, NULL);
        //if (diff_ms(end, start) > 1200)
            //moveWindow(&hero);

}

void PlayState::Draw(GameEngine* game) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glTranslatef(0.0f, 0.0f, -5.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glVertex2i(-10,-10);
    glVertex2i(-10, 10);
    glVertex2i( 10, 10);
    glVertex2i( 10,-10);
    glEnd();

    //render background
    //render objects
//void render(Object *hero){
    /*
    float w, h, tl_sz, x, y;
    x = roomX - (WINDOW_WIDTH/2);
    y = roomY - (WINDOW_HEIGHT/2);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3ub(65,155,225);
    // render grounds
    Object *ground;
    for (int i=0;i<grounds_length;i++){
        ground = grounds[i];
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
    // render enemies
    glColor3ub(100,0,0);
    Object *enemy;
    for (int i=0;i<enemies_length;i++){
        enemy = enemies[i];
        //Ground
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
    //Non-Collision Object
    */
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


/*
    // Draw Hero Sprite
    glPushMatrix();
    glTranslatef( hero.getCenterX() - x, hero.getCenterY() - y, 0);
    w = hero.getWidth();
    h = hero.getHeight();
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    tl_sz = 0.076923077;
    if ((hero.getVelocityX() < 0.0) or (hero.getOldCenterX()>hero.getCenterX())){
        glTexCoord2f((hero.getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f((hero.getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero.getIndex()*tl_sz), 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero.getIndex()*tl_sz), 1.0f); glVertex2i(w,-w);
    } else {
        glTexCoord2f(hero.getIndex()*tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f(hero.getIndex()*tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((hero.getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(w,w);
        glTexCoord2f((hero.getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(w,-w);
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

//}
// render background
//void renderBackground(){
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
        int j = bit->pos[2];
        int i;
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
    */
//}
}

// time difference in milliseconds
int PlayState::diff_ms(timeval t1, timeval t2) {
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec))/1000;
}

bool PlayState::detectCollide(Object *hero, Object *ground){
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (  hero->getRight()  > ground->getLeft() &&
            hero->getLeft()   < ground->getRight() &&
            hero->getBottom() < ground->getTop()  &&
            hero->getTop()    > ground->getBottom()
           );
}

void PlayState::groundCollide(Object *hero, Object *ground){
    //(Moving Object, Static Object)
    //Detects Which boundaries the Moving Object is around the Static Object
    //top,down,left,right
    if(detectCollide(hero, ground)){
        s_right=hero->getRight();
        s_left=hero->getLeft();
        s_top=hero->getTop();
        s_bottom=hero->getBottom();
        g_right=ground->getRight();
        g_bottom=ground->getBottom();
        g_top=ground->getTop();
        g_left=ground->getLeft();
        //If moving object is on top of the static object
        if(!(hero->getOldBottom() < g_top) &&
                !(s_bottom >= g_top) && (hero->getVelocityY() < 0)){
            hero->setVelocityY(0);
            hero->setCenter(hero->getCenterX(),
                    g_top+(hero->getCenterY()-s_bottom)
                    );
            isFalling=isJumping=didJump=0;
        }
        //If moving object is at the bottom of static object
        if(!(hero->getOldTop() > g_bottom) &&
                !(s_top <= g_bottom)){
            hero->setVelocityY(-0.51);
            hero->setCenter(hero->getCenterX(),
                    g_bottom-(s_top-hero->getCenterY())
                    );
        }
        //If moving object is at the l-eft side of static object
        if(!(hero->getOldRight() > g_left ) &&
                !(s_right <= g_left)){
            hero->setVelocityX(-0.51);
            hero->setCenter(g_left-(s_right-hero->getCenterX()),
                    hero->getCenterY()
                    );
        }
        //If moving object is at the right side of static object
        if(!(hero->getOldLeft() < g_right ) &&
                !(s_left >= g_right)){
            hero->setVelocityX(0.51);
            hero->setCenter(g_right+(hero->getCenterX()-s_left),
                    hero->getCenterY()
                    );
        }
    }
}

void PlayState::movement(Object *hero){
    Object *ground;
    for (int i=0; i<grounds_length; i++){
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

Object PlayState::createAI( int w, int h, Object *ground) {
    Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
    //cout << glGetIntegerv(GL_VIEWPORT);
    return newEnemy;

}

void PlayState::moveWindow(Object *hero) {
    double heroWinPosX = hero->getCenterX();
    double heroWinPosY = hero->getCenterY();

    //move window forward
    if (heroWinPosX > roomX + interval) {
        roomX+=5;
    }
    //move window backward (fast move if hero is far away)
    else if ((heroWinPosX < roomX - interval) && roomX>(WINDOW_WIDTH/2)) {
        roomX-=5;
            if (heroWinPosX < (roomX - interval - 400)){
                roomX-=20;
            }
            if (heroWinPosX < (roomX - interval - 800)){
                roomX-=50;
            }
    }
    //move window up
    if (heroWinPosY > roomY + interval) {
        roomY+=5;
    }
    //move window down
    else if ((heroWinPosY < roomY - interval) && roomY>(WINDOW_HEIGHT/2)) {
        int i = hero->getVelocityY();
        if (i>-5)
            i=-5;
        roomY+=i;
    }

    // TODO: update parameter to reflect the actual size of level.
    else if(heroWinPosX > 5000) {
        return;
    }
}

unsigned char* PlayState::buildAlphaData(Ppmimage *img){
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


