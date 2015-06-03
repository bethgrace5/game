#include <sys/time.h>
#include <GL/glx.h>
#include "definitions.h"
#include "functions.h"
#include "Sprite.h"
#include "Object.h"
#include "Player.h"
#include <iostream>

#ifdef USE_SOUND
#include "sounds.h"
#endif

//==============================================
//Setup
//==============================================
Player::Player() : Object(26, 44, 250, 250), Sprite(){
  //Set The Default stats
  lives = 3; 
  health =  maxHealth = 100;
  isShooting=0;
  jumps = 0; jumpLimit = 2; jumpPower = 7;
  speed = 0; maxSpeed  = 7; speedRate = 1;
  gunType = -1;
  invincible = 0;
  isShooting=0;
  Sprite::setMirror(false);
  indexp = 0; once = 0;
}
//===============================================
//Movement Functions
//===============================================
void Player::moveRight(){ 
//  if(getVelocityX() >= maxSpeed) return;
  Sprite::setMirror(false);
  isWalking = 1;
  Object::setVelocityX(maxSpeed);
}
void Player::moveLeft(){
//  if(getVelocityX() <= -maxSpeed) return;

  Sprite::setMirror(true);
  isWalking = 1;
  Object::setVelocityX(-maxSpeed);
}
void Player::jump(){
  if(jumps < jumpLimit){ 
      Object::setVelocityY(jumpPower); 
      #ifdef USE_SOUND
      fmod_playsound(jumpSound);
      #endif
      jumps++;
  }
}
void Player::stop(){
  isWalking = 0;
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
  if(invincible){ 
    std::cout << "test\n"; 
    return;
  }
  health -= take;
}
void Player::repairHealth(int take){
  health += take;
  if(health > maxHealth) health = 100;
}

void Player::setInvisible(bool take){
  invisible = take;
}
void Player::setInvincible(bool take){
  invincible = take;
}

void Player::setGunType(int take){
  gunType = take;
}

bool Player::checkShooting(){
  return Object::isShooting;
}
void Player::setShooting(bool take){ 
    Object::isShooting = take;
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
void Player::reset(){ 
  health =  maxHealth = 100;
  isShooting=0;
  lives = 3;
  health = 100;
  isShooting=0;
  Sprite::setMirror(false);
  setVelocityX(0);
  setVelocityY(0);
  setCenter(200, 400);
  lives = 3; 
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
              //reset lives
              lives = 3;
              //kick user back to menu
              level = 0;
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
  /*
  if(!isWalking){
    std::cout << "walking\n";
    if(getVelocityX() < 0) setVelocityX(getVelocityX()+1);
    else if(getVelocityX() > 0) setVelocityX(getVelocityX()-1);
    else setVelocityX(0);
  }*/
  

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
/*int Player::diff_ms (timeval t1, timeval t2) {
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}*/
