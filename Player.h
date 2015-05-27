#include "definitions.h"
#include "Sprite.h"

#ifdef USE_SOUND
#include "fmod.c"
#include "sounds.cpp"
#endif

#ifndef PLAYER_H
#define PLAYER_H
//=====================================================================
//Player
//=====================================================================
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
//==============================================
//Setup
//==============================================
Player::Player() : Object(26, 44, 250, 250), Sprite(){
  //Set The Default stats
  lives = 3; 
  health =  maxHealth = 100;

  jumps = 0; jumpLimit = 2; jumpPower = 7;
  speed = 0; maxSpeed  = 7;

  indexp = 0; once = 0;
}
//===============================================
//Movement Functions
//===============================================
void Player::moveRight(){ 
  Sprite::setMirror(false);
  Object::setVelocityX(maxSpeed);
}
void Player::moveLeft(){
  Sprite::setMirror(true);
  Object::setVelocityX(-maxSpeed);
}
void Player::jump(){
  if(jumps < jumpLimit){ 
      Object::setVelocityY(jumpPower); 
      #ifdef USE_SOUND
      fmod_playsound(marioJump);
      #endif
      jumps++;
  }
}
void Player::stop(){
  Object::setVelocityX(0);
}
void Player::jumpRefresh(){ 
  if(Player::getVelocityY() == 0) jumps = 0; 
}
//==============================================
//Stats Functions
//==============================================
void Player::setHealth(int take){
  health = take;
}
int Player::getHealth(){
  return health;
}
void Player::reduceHealth(int take){
  health -= take;
}
void Player::repairHealth(int take){
  health += take;
  if(health > maxHealth) health = 100;
}

bool Player::checkShooting(){
  return triggerShooting;
}
void Player::setShooting(bool take){ 
  triggerShooting = take;
}

int Player::getLives(){
  return lives;
}
void Player::incrementLives(){ 
    lives++;
}
void Player::decrementLives(){ 
    lives--;
    // cycleAnimations() checks for 0 or less health
    // to show dying sequence
    if(lives < 0) {
        health =0;
    }
}

//===============================================
//Drawing Functions
//===============================================/
//    Note: This is a Prototype -
//          -Right now its specific number depedant.
//          -
//
//       Right Now There Are 13 sprites in 1 sheet
void Player::cycleAnimations(){
  //int tmpColumn = Sprite::getColumn();//
  //int tmpRow = Sprite::getRow();

  //Start Timer
  if(once == 0){ gettimeofday(&seqStartA, NULL); once = 1 ;}
  gettimeofday(&seqEndA, NULL);//This Can Be Universal For All Stuff so Maybe Make This
                               //Global in main.cpp
  //The Earilier Animations States gets first priority

  //Death
  if(getHealth() <=0){

    if(diff_ms(seqEndA, seqStartA) > 180){
      if(indexp == 12) {
          if (lives < 0) {
              lives = 3;
          }
          return;
      }
      if(indexp < 7) indexp = 7;
      indexp++; once = 0;
    }
  }
  //Jumping
  else if(Object::getVelocityY() > 0){
      if(jumps==2) {
          indexp=0;
      }
      else {
        indexp = 1;
      }
  }
  //Falling
  else if(Object::getVelocityY() < 0){
    indexp = 0;
  }
  //Walking/Running
  else if(Object::getVelocityX() != 0){
    if(diff_ms(seqEndA, seqStartA) > 80){
      indexp = (indexp + 1) % 6;
      once = 0;
    }
  }
  //Standing
  else indexp = 6; 
  //std::cout << "Index: " << indexp << endl;
  Sprite::setIndexAt(indexp);
}

void Player::autoState(){
  //This Is a Temporary Fix. For The Enemy AI
  if(getVelocityY() < 0) Object::isJumping = 1;
  else Object::isJumping = 0;

  if(getVelocityX() < 0) Object::isWalking = 1;
  else Object::isWalking = 0;

  if(getVelocityY() > 0) Object::isFalling = 1;
  else Object::isFalling = 0;
}

void Player::drawBox(){
  //int w = Object::getWidth();
  //int h = Object::getHeight();
  glPushMatrix();
  glTranslatef(Object::getCenterX(), Object::getCenterY(), 0);
/*
  glBegin(GL_QUADS);
  glVertex2i(-w, -h);
  glVertex2i(-w,  h);
  glVertex2i( w,  h);
  glVertex2i( w, -h);
  glEnd();*/

  //Sprite::drawTile(Sprite::getIndexAt());
  Sprite::drawTile(indexp, 1);

  glPopMatrix();
}
//==============================================
//Helper Functions
//==============================================
int Player::diff_ms (timeval t1, timeval t2) {
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}
#endif
