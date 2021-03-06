
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
    int direction;
    int byo;
    int attackSound;
    int soundCollide;

    bool stop;
    bool start; 
    bool timeBase; //If This Is On | Attack Will Last on its Duration
    bool cycleBase;//If This is On | Attack Will Last until the 
                   // end of the sprite sheet
    int indexp;  
 
    int damage;
    int charges;
    int pushForce;
    int backForce;
    bool pulling;
    bool infiniteCharges;
    bool singleUse;
    bool constantEffect;
    bool effectEnemy;
    bool effectPlayer;
    bool pushAway;
    bool pushBack;
    bool invincible;
    bool invisiblity;

    bool hurtOnce;
    
    bool stickOn;
    bool moveWith;

    timeval seqStartA, seqEndA;
    timeval timeIn, timeOut;

  public:  
    Attack();
    ~Attack();

    Object *Clone(){ return new Attack(*this); }

    void targetAt(Object *target);
    void referenceTo(Sprite take, int id);
    int  checkSpriteID();
    void fakeCopy(Attack * other);

    void changeRate(int take);
    void setDuration(int take);
    void checkDuration();

    void setTimeBase(bool take);
    void setCycleBase(bool take);
    void setStickOn(bool take);
    void setMoveWith(bool take);
    void setPushAway(bool take);
    void setPushBack(bool take);

    void setForwardForce(int take);
    void setBackForce(int take);
    void setPulling(bool take);

    void setEffectEnemy(bool take);
    void setEffectPlayer(bool take);
    void setConstantEffect(bool take);
    void setCharges(int take);
    void setInfiniteCharges(bool take);
    void setInvincible(bool take);
    void setInvisiblity(bool take);
    void setDamage(int take);
    void returnVelocity();
    
    bool checkStop();
    void setSound(int take);
    int getSound();
    void setAttackSound(int take);
    int getAttackSound();
    void setSoundCollide(int take);
    int getSoundCollide();

    void autoState();
    void stickOnHero();
    void moveWithHero();
    void pushAwayForce(Object *obj);
    void pushBackSelf();
    void pullingIn(Enemy *enemy);

    void causeEffect(Player *hero);
    void causeEffect(Enemy *enemy);

    void cycleAnimations();

    void drawBox(Sprite targetSprite);
};

#endif

