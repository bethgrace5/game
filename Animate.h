#include "definitions.h"
#include "Sprite.h"

#ifdef USE_SOUND
#include "fmod.c"
#include "sounds.cpp"
#endif

#ifndef Animate_H
#define Animate_H

#define FRAMEMAX 25
//=====================================================================
//Player
//=====================================================================
class Animate: public Sprite{
  private:
    struct indexA{
     int x, y;
    } frame[FRAMEMAX];

    int once;
    int frameRate;
    int timer;
    int indexp;
    timeval seqStartA, seqEndA;
    timeval timeIn, timeOut;

  public:
    Animate();

    void cycleAnimations();
    void changeRate(int take);
    void drawBox();

    int diff_ms (timeval t1, timeval t2);
};
//==============================================
//Setup
//==============================================
Animate::Animate(): Sprite(){
  once = 0;
  timer = 0;
  indexp = 0;
  frameRate = 55;
  //for(int i = 0; i < FRAMEMAX; i++){
  //  frame[i].x = frame[i].y = -1; 
  //}
  //Set The Default stats
}

void Animate::changeRate(int take){
  frameRate = take;
}
//===============================================
//Drawing Functions
//===============================================/
//    Note: This is a Prototype -
//          -Right now its specific number depedant.
//          -
//
//       Right Now There Are 13 sprites in 1 sheet

void Animate::cycleAnimations(){
  //int tmpColumn = Sprite::getColumn();//
  //int tmpRow = Sprite::getRow();
  //Start Timer
  if(once == 0){ gettimeofday(&seqStartA, NULL); once = 1 ;}
  gettimeofday(&seqEndA, NULL);

  if(diff_ms(seqEndA, seqStartA) > frameRate){
      //indexp = (indexp + 1) % Sprite::getRow()*Sprite::getColumn();
      indexp++; 
      if(indexp > Sprite::getRow() * Sprite::getColumn()) indexp = 0;
      once = 0;
  }
  Sprite::setIndexAt(indexp);
}

void Animate::drawBox(){
  glPushMatrix();

  Sprite::drawTile(indexp);

  glPopMatrix();
}
//==============================================
//Helper Functions
//==============================================
int Animate::diff_ms (timeval t1, timeval t2) {
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}
#endif
