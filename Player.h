#ifndef PLAYER_H
#define PLAYER_H

#include <sys/time.h>
#include <GL/glx.h>
#include "definitions.h"
#include "Sprite.h"
#include "Object.h"

#ifdef USE_SOUND
#include "sounds.h"
#endif

//=====================================================================
//Player
//=====================================================================
extern int level;
class Player: public Object, public Sprite{
  private:
    int lives, health, maxHealth;
    int jumps, jumpLimit, jumpPower;
    int maxSpeed, speed;

    int indexp;
    int once;

    bool triggerShooting;

    timeval seqStartA, seqEndA;

  public:
    Player();
    Player(int width, int height, int x, int y);

    void moveLeft();
    void moveRight();
    void jump();
    void stop();
    void jumpRefresh();

    int getHealth();
    void setHealth(int take);
    void reduceHealth(int take);
    void repairHealth(int take);

    bool checkShooting();
    void setShooting(bool take);
    int getLives();
    void incrementLives();
    void decrementLives();

    void drawBox();
    void cycleAnimations();
    void setOnGround(bool take);
    void autoState();

    int diff_ms (timeval t1, timeval t2);

};
#endif
