//Chad Danner
//cs335
//HW3 
//05-11-15

//  Platform_Functions
//  Attack_Functions

#include <GL/glx.h>

#include "ppm.h"
#include "Object.h"
#include "Sprite.h"
#include "Player.h"
#include "chadD.h"
#include "Item.h"

extern Player *hero;

#ifdef USE_SOUND
#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "fmod.h"
#include "sounds.h"
#endif

using namespace std;
//=====================================================================
//  Platform_Functions
//=====================================================================
Platform::Platform() : Sprite(), Object(260, 200, 350, 400) {
  lineSpaceX = lineSpaceY = 0;
}

void Platform::setupTile() {
  //Setup Object to match with tile rows
  //The Object Will Allocate to fit with the tile
  int widthSize;
  int heightSize;

  //The Object will resize itself to fit with rows of tile. (by Downsizing)
  float addWidth = (int)Object::getWidth() % (int)Sprite::getClipWidth();
  float addHeight = (int)Object::getHeight() % (int)Sprite::getClipHeight();

  widthSize = Object::getWidth() - addWidth; 
  heightSize = Object::getHeight() - addHeight; 

  Object::init(widthSize, heightSize, Object::getCenterX(), Object::getCenterY());  

  saveLineSpace();
}

void Platform::saveLineSpace() {
  lineSpaceX = Sprite::getClipWidth();
  lineSpaceY = Sprite::getClipHeight();
}

void Platform::drawRow(int x, int y) {
  //Draws Tiles for a box
  //This is To Test The Boundaries Of The Platform
  //you will see a colored box 
  /*float w, h; 
  glPushMatrix();
  glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
  w = Object::getWidth();
  h = Object::getHeight();

  glBegin(GL_QUADS);
  glVertex2i(-w, -h);
  glVertex2i(-w, h);
  glVertex2i( w, h);
  glVertex2i( w,-h);
  glEnd();
  glPopMatrix();
  */

  //This Will Draw The TileSet based on the Boundaries of Object
  glPushMatrix();
  glTranslatef(Object::getLeft(), Object::getTop(), 0);
  for(int j = 0; j < Object::getHeight()/lineSpaceY; j++) {
    if (j == 0) glTranslatef(0, -lineSpaceY , 0); 
    else        glTranslatef(0, -(lineSpaceY + lineSpaceY), 0); 

    glPushMatrix();
    for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {
      if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
      else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 
      drawTile();
    }
    glPopMatrix();
  } 
  glPopMatrix(); 
}

//=====================================================================
//  Attack_Core_Setup
//=====================================================================
Attack::Attack() : Object(0, 0, 0, 0){
  once = onceOnly = singleUse = 0;
  indexp = 0;
  damage = 5;
  frameRate = 0;
  hurtOnce = 0;
  duration = 1000;
  cycleBase = 1;
  timeBase = 0;
  stop = 0; 
  start = 0;
  stickOn = 0;
  moveWith = 0;
  pushAway = 0;
  pushBack = 0;
  effectEnemy = 0;
  effectPlayer = 0;
  charges = 1;
  constantEffect = 0;
 
  invincible = 0; 
  attackSound = 1;
  soundCollide = 1;
}

Attack::~Attack(){
  std::cout << "did this work\n"; 
  if(invincible){
    std::cout << "Invinciblity Gone\n"; 
    hero->setInvincible(false); 
  }
}
//=====================================================================
//  Setup
//=====================================================================
void Attack::setAttackSound(int take){
  attackSound = take;
}
int Attack::getAttackSound(){
    return attackSound;
}
void Attack::setSoundCollide(int take){
  soundCollide = take;
}
int Attack::getSoundCollide(){
    return soundCollide;
}

void Attack::referenceTo(Sprite take, int id){
  rowAt = take.getRow();
  columnAt = take.getColumn();
  spriteID = id;
}
int Attack::checkSpriteID(){
  return spriteID;
}
//==============================================
//Rates Functions
//==============================================
void Attack::changeRate(int take){
  frameRate = take;
}
void Attack::setDuration(int take){
  duration = take;
}
bool Attack::checkStop(){
  return stop;
}
void Attack::causeEffect(Enemy *enemy){
  if(!effectEnemy) return;
  if(charges <= 0) return;
  pushAwayForce(enemy);
  enemy->life-=damage;
  charges--;
}

void Attack::causeEffect(Player *hero){
  if(!effectPlayer) return;  
  if(charges <= 0) return;
  pushAwayForce(hero);
  hero->reduceHealth(damage);
  charges--;
}
//=============================================
//Setup Behavior
//=============================================
void Attack::setTimeBase(bool take){
  timeBase = take; 
}
void Attack::setCycleBase(bool take){
  cycleBase = take;
}
void Attack::setStickOn(bool take){
  stickOn = take;
}
void Attack::setMoveWith(bool take){
  moveWith = take;
}
void Attack::setPushAway(bool take){
  pushAway = take;
}
void Attack::setPushBack(bool take){
  pushBack = take;
}
void Attack::setEffectEnemy(bool take){
  effectEnemy = take;
}
void Attack::setEffectPlayer(bool take){
  effectPlayer = take;
}
void Attack::setDamage(int take){
  damage = take;
}
void Attack::setConstantEffect(bool take){
  constantEffect = take;
}
void Attack::setCharges(int take){
  charges = take;
}
void Attack::setInvincible(bool take){
  invincible = 1;
}
//==============================================
// Behavior_Functions
//==============================================
void Attack::targetAt(Object *caster){
  target = caster;
}

void Attack::autoState(){
  if(timeBase) checkDuration();
  if(cycleBase) if(indexp == 0 && start) stop = 1; 
  if(charges == 0) stop = 1;

  if(stickOn) stickOnHero();
  if(moveWith) moveWithHero();
  if(singleUse == 0){ 
    if(pushBack) pushBackSelf();
    if(invincible) hero->setInvincible(true);
    singleUse = 1;
  }

  //Follow Player Movement
  //Affect By Gravity
  //Directional Shots
  if(stop && moveWith){

    //target->setVelocityX(0);
  }
}

void Attack::stickOnHero(){
  Object::setCenter(target->getCenterX(), target->getCenterY());
}

void Attack::moveWithHero(){
    target->setVelocityX(Object::getVelocityX());
    target->setVelocityY(Object::getVelocityY());
}

void Attack::checkDuration(){
  if(onceOnly == 0){ gettimeofday(&timeOut, NULL); onceOnly = 1;}
  gettimeofday(&timeIn, NULL);
  if(diff_ms(timeIn, timeOut) > duration){
    stop = 1;
  }
}

void Attack::pushAwayForce(Object *obj){
  //obj->setVelocityX(getVelocityX());
  //obj->setVelocityY(getVelocityY());
  obj->setCenter(obj->getCenterX() + getVelocityX(), obj->getCenterY());
}
void Attack::pushBackSelf(){
  target->setVelocityX(-(getVelocityX()));
  target->setVelocityY(5);
}

//===============================================
//Drawing Functions
//===============================================/
//    Note: This is a Prototype -
//          -Right now its specific number depedant.
void Attack::cycleAnimations(){
  if(once == 0){ gettimeofday(&seqStartA, NULL); once = 1;}
  gettimeofday(&seqEndA, NULL);

  if(diff_ms(seqEndA, seqStartA) > frameRate){
    indexp++;
    if(indexp > rowAt * columnAt) indexp = 0;
    once = 0; start = 1;
  }
}

void Attack::drawBox(Sprite targetSprite){
  if(stop == 1) return;

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
  glEnd();
  */

  targetSprite.drawTile(indexp);

  glPopMatrix();
}
