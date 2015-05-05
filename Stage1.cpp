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
#include "Stage1.h"
#include "IntroState.h"
#include "fonts.h"
#include "Object.h"
#include "fastFont.h"
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

Stage1 Stage1::m_Stage1;
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

void Stage1::Init() {
    gettimeofday(&start, NULL);
    srand(time(NULL));

    //declare hero object
    hero.setTop(44);
    hero.setBottom(-44);
    hero.setLeft(-26);
    hero.setRight(26);

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
    //initialize_fonts();
    initFastFont();
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

void Stage1::Cleanup() {
    //clean background
    bgBit *s;
    while (bitHead) {
        s = bitHead->next;
        delete bitHead;
        bitHead = s;
    }
    bitHead = NULL;
}

void Stage1::Pause() {
}

void Stage1::Resume() {
}

void Stage1::HandleEvents(GameEngine* game) {
    int done = 0;
        if(XPending(game->dpy)) {
            while(!done) {
                //Player User Interfaces
                XEvent e; XNextEvent(game->dpy, &e);
                check_mouse(&e);
                done = check_keys(&e, &hero, game);
            }
    }
      
}

int Stage1::check_mouse(XEvent *e) {

    static int savex = 0, savey = 0;
    //static int n = 0;
    if (e->type == ButtonRelease) { return 0;}
    if (e->type == ButtonPress) {
        if (e->xbutton.button==1) { //Left button was pressed
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
int Stage1::check_keys(XEvent *e, Object *hero, GameEngine *game){
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
            cout << "hit a in Play State\n";
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
            hero->setVelocityX(0);
            game->ChangeState( IntroState::Instance() );
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
}

void Stage1::Update(GameEngine* game) {
    // check object movement, move window
        movement(&hero);
        moveWindow(&hero);
}

void Stage1::Draw(GameEngine* game) {
    // color background
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glTranslatef(150, 150, 0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(20, 30, 40);

    float x = roomX - (WINDOW_WIDTH/2);
    float y = roomY - (WINDOW_HEIGHT/2);
    glClear(GL_COLOR_BUFFER_BIT);

    drawGround(x, y);
    drawFonts();
    drawEnemy(x, y);
    drawHero(x, y);
    drawBackground();
    glXSwapBuffers(game->dpy, game->win);
}

void Stage1::drawGround(int x, int y){
    float w, h;
    Object *ground;
    glColor3ub(65,155,225);
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
}
void Stage1::drawEnemy(int x, int y) {
    float w, h;
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

}

void Stage1::drawHero(int x, int y) {
    // Draw Hero Sprite
    glPushMatrix();
    glTranslatef( hero.getCenterX() - x, hero.getCenterY() - y, 0);
    float w = hero.getWidth();
    //float h = hero.getHeight();
    glBindTexture(GL_TEXTURE_2D, heroTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_LESS, 1.0f);
    glColor4ub(255,255,255,255);
    glBegin(GL_QUADS);
    float tl_sz = 0.076923077;
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
}

void Stage1::drawFonts() {
    writeWords("LIVES", 32, WINDOW_HEIGHT-32);
    if (fail>0){
        writeWords("FAIL", WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
        fail--;
    }
}

void Stage1::drawBackground() {
// render background
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
            //ggprint12(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
            writeWords((bit->vel[2]>0.5?"1":"0"), i*65536+256*i+i, 100);
        } else if (j>0.9) {
            //ggprint10(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
            //writeWords("LIVES", i*65536+256*i+i, i);
        } else {
            //ggprint08(&r0, 16, i*65536+256*i+i, (bit->vel[2]>0.5?"1":"0") );
            //writeWords("LIVES", i*65536+256*i+i, i);
        }
        bit = bit->next;
    }
    glLineWidth(1);
}

// time difference in milliseconds
int Stage1::diff_ms(timeval t1, timeval t2) {
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec))/1000;
}

bool Stage1::detectCollide(Object *obj1, Object *obj2){
    //Gets (Moving Object, Static Object)
    //Reture True if Moving Object Collides with Static Object
    return (  obj1->getRight()  > obj2->getLeft() &&
                obj1->getLeft()   < obj2->getRight() &&
                obj1->getBottom() < obj2->getTop()  &&
                obj1->getTop()    > obj2->getBottom()
               );
}

void Stage1::groundCollide(Object *obj, Object *ground){
    //(Moving Object, Static Object)
    //Detects Which boundaries the Moving Object is around the Static Object
    //top,down,left,right
    if(detectCollide(obj, ground)){
        s_right=obj->getRight();
        s_left=obj->getLeft();
        s_top=obj->getTop();
        s_bottom=obj->getBottom();
        g_right=ground->getRight();
        g_bottom=ground->getBottom();
        g_top=ground->getTop();
        g_left=ground->getLeft();
        //If moving object is on top of the static object
        if(!(obj->getOldBottom() < g_top) &&
                !(s_bottom >= g_top) && (obj->getVelocityY() < 0)){
            obj->setVelocityY(0);
            obj->setCenter(obj->getCenterX(),
                    g_top+(obj->getCenterY()-s_bottom)
                    );
            isFalling=isJumping=didJump=0;
        }
        //If moving object is at the bottom of static object
        if(!(obj->getOldTop() > g_bottom) &&
                !(s_top <= g_bottom)){
            obj->setVelocityY(-0.51);
            obj->setCenter(obj->getCenterX(),
                    g_bottom-(s_top-obj->getCenterY())
                    );
        }
        //If moving object is at the l-eft side of static object
        if(!(obj->getOldRight() > g_left ) &&
                !(s_right <= g_left)){
            obj->setVelocityX(-0.51);
            obj->setCenter(g_left-(s_right-obj->getCenterX()),
                    obj->getCenterY()
                    );
        }
        //If moving object is at the right side of static object
        if(!(obj->getOldLeft() < g_right ) &&
                !(s_left >= g_right)){
            obj->setVelocityX(0.51);
            obj->setCenter(g_right+(obj->getCenterX()-s_left),
                    obj->getCenterY()
                    );
        }
    }
}

void Stage1::movement(Object *hero){
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

Object Stage1::createAI( int w, int h, Object *ground) {
    Object newEnemy(w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h);
    //cout << glGetIntegerv(GL_VIEWPORT);
    return newEnemy;

}

void Stage1::moveWindow(Object *hero) {
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
    else if(heroWinPosX > 5000) {
        return;
    }
}

unsigned char* Stage1::buildAlphaData(Ppmimage *img){
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

