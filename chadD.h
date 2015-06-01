
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
    Object *target;

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
    int indexp;  

    int damage;
    bool effectEnemy;
    bool effectPlayer;

    bool hurtOnce;
    
    bool stickOn;
    bool moveWith;

    timeval seqStartA, seqEndA;
    timeval timeIn, timeOut;

  public:  
    Attack();

    Object *Clone(){ return new Attack(*this); }

    void targetAt(Object *target);
    void referenceTo(Sprite take, int id);
    void fakeCopy(Attack * other);

    void changeRate(int take);
    void setDuration(int take);
    void checkDuration();

    void setTimeBase(bool take);
    void setCycleBase(bool take);
    void setStickOn(bool take);
    void setMoveWith(bool take);
    void setEffectEnemy(bool take);
    void setEffectPlayer(bool take);
    void changeDamage(int take);
    void returnVelocity();
    
    bool checkStop();

    void autoState();
    void direction();
    void stickOnHero();
    void moveWithHero();

    void causeEffect(Player *hero);
    void causeEffect(Enemy *enemy);

    void cycleAnimations();

    void drawBox(Sprite targetSprite);
};

#endif

