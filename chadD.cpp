//Chad Danner
//cs335
//HW3 
//05-11-15

#include <GL/glx.h>

#include "ppm.h"
#include "Object.h"
#include "Sprite.h"
#include "Player.h"
#include "Platform.h"
#include "Item.h"


using namespace std;

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
//Attack
//=====================================================================
Attack::Attack() : Object(0, 0, 0, 0){
  once = onceOnly = 0;
  timer = 1;
  indexp = 0;
  damage = 5;
  frameRate = 55;
  hurtOnce = 0;
  duration = 1000;
  cycleBase = 1;
  timeBase = 0;
  stop = 0; 
  start = 0;
}

void Attack::referenceTo(Sprite take, int id){
  rowAt = take.getRow();
  columnAt = take.getColumn();
  spriteID = id;
}

//==============================================
//Stats Functions
//==============================================
void Attack::changeRate(int take){
  frameRate = take;
}

void Attack::changeDuration(int take){
  duration = take;
}

void Attack::causeEffect(Player *hero){
  hero->reduceHealth(damage);
}

bool Attack::checkStop(){
  return stop;
}

void Attack::causeEffect(Enemy *singleEnemy){
  if(timer == 0) return;

  std::cout << "Enemy Got Hit\n";
}
//==============================================
//
//==============================================
void autoState(){
  
  //Follow Player Movement
  //Affect By Gravity
  //Directional Shots

}
//===============================================
//Drawing Functions
//===============================================/
//    Note: This is a Prototype -
//          -Right now its specific number depedant.
void Attack::cycleAnimations(){
  if(once == 0){ gettimeofday(&seqStartA, NULL); once = 1;}
  gettimeofday(&seqEndA, NULL);

  //if(timeBase){
  //  checkDuration();
  //}

  if(diff_ms(seqEndA, seqStartA) > frameRate){
    indexp++;
    if(indexp > rowAt * columnAt) indexp = 0;
    //if(indexp > 8) indexp = 0;

    once = 0; start = 1;
  }
  //std::cout << "indexp " << indexp << "\n";

  if(cycleBase){
    if(indexp == 0 && start) stop = 1; 
  }
}

void Attack::checkDuration(){
  if(onceOnly == 0){ gettimeofday(&timeOut, NULL); onceOnly = 1;}
  std::cout << "test\n";
  gettimeofday(&timeIn, NULL);
  if(diff_ms(timeIn, timeOut) > duration){
    stop = 1;
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
//==============================================
//Helper Functions
//==============================================
