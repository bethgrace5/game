
#ifndef PLATFORM_H
#define PLATFORM_H

//Chad Danner
//cs335
//HW3 
//05-11-15

//=====================================================================
//  Platform
//=====================================================================
//How To Use...
//
////Global
//  Platform var1; //This Sets Up The Platform Varaible
//
//  within init_opengl function // in main.cpp
//    add
//    var1.insert("./images/"ppm file"", columns, rows);
//    var1.init(width, height, x-cord, y-cord);
//    var1.setupTile();
//
//  within the movement function
//    groundCollide(hero, &var1);
//
//  within the render function
//    var1.drawRow(0,0);
//

#include "Sprite.h"
#include "Object.h"
#include "Enemy.h"
#include "Player.h"
#include <iostream>

extern Player *hero;
extern Enemy *Enemies[MAX_ENEMIES];

class Platform : public Sprite, public Object {

  private:
    int lineSpaceX;
    int lineSpaceY;

  public:
    Platform();
    void setupTile();
    void drawRow();
    void saveLineSpace();
    void drawRow(int x, int y);
};

class Attack: public Object {
  private:
    int spriteID;
    int rowAt;
    int columnAt;
  
    int timer;
    int frameRate;
    int duration;
    int once;
    int cycle;
    int onceOnly;

    bool stop;
    bool start; 
    bool timeBase; //If This Is On | Attack Will Last on its Duration
    bool cycleBase;//If This is On | Attack Will Last until the 
                   // end of the sprite sheet
                   //
    int indexp;  

    int damage;
    bool hurtOnce;
    
    bool stickOn;

    timeval seqStartA, seqEndA;
    timeval timeIn, timeOut;

  public:  
    Attack();

    Object *Clone(){ return new Attack(*this); }

    void referenceTo(Sprite take, int id);
    void fakeCopy(Attack * other);

    void changeRate(int take);
    void changeDuration(int take);
    void checkDuration();
    bool checkStop();

    void autoState();
    void stickOnHero();

    void causeEffect(Player *hero);
    void causeEffect(Enemy *Enemy);

    void cycleAnimations();

    void drawBox(Sprite targetSprite);
};

#endif

