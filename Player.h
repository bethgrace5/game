#ifndef PLAYER_H
#define PLAYER_H

#include <sys/time.h>
#include <GL/glx.h>
#include "definitions.h"
#include "functions.h"
#include "Sprite.h"
#include "Object.h"

//=====================================================================
//Player
//=====================================================================
extern int level;
class Player: public Object, public Sprite{
  private:
    int lives, health, maxHealth;
    int jumps, jumpLimit, jumpPower;
    int maxSpeed, speed, speedRate;

    int ammo;
    int indexp;
    int once;

    //bool triggerShooting;
    int gunType;
    bool invincible;
    bool invisible;
    

    bool coolDownTimer;
    int coolDownLength;
    timeval seqStartA, seqEndA;
    timeval refreshStart, refreshEnd;

  public:
    Player();
    Player(int width, int height, int x, int y);

    void moveLeft();
    void moveRight();
    void jump();
    void stop();
    void jumpRefresh();
    bool coolDowns();

    int getHealth();
    void setHealth(int take);
    void reduceHealth(int take);
    void repairHealth(int take);
    void setInvincible(bool take);
    void setInvisible(bool take);
    
    bool checkShooting();
    void setShooting(bool take);
    void setGunType(int take);
    int  checkGunType();
    void setAmmo(int take);
    int checkAmmo();
    void decreaseAmmo(int ammount);
    int getLives();
    void incrementLives();
    void decrementLives();
    void reset();

    void drawBox();
    void cycleAnimations();
    void setOnGround(bool take);
    void autoState();

    //int diff_ms (timeval t1, timeval t2);
};
#endif
