#include "Item.h"
#include "Sprite.h"
#include "Player.h"
#include "Player.h"
#include "Enemy.h"
#include "chadD.h"
  #ifdef USE_SOUND
  #include "sounds.h"
  #endif
#include "AttackList.h"
#include <stdlib.h>

extern attack_list boxA;
extern Player *hero;

extern int fireUp;
extern int fireDown;
extern int pushingLaser;
extern int fireShield;
extern int speedArrow;
extern int shield;
extern int simpleBlast;
extern int gravityBlast;

Item::Item() : Sprite(), Object(20, 20, 350, 350) {
  //effect=3;
  effect = rand() % 2 + 1;
  end = 0;
}

Item::Item(int take): Sprite(), Object(20,20,0,0){
  effect = take;
  end = 0;
}

void Item::causeEffect(Player *hero) {
  hero->setAmmo(10);
  switch (effect) {
    case 1: 
      hero->repairHealth(25);
      //fireUp += 3;
      break;
    case 2:
      hero->repairHealth(50);
      //boxA.copyAttack(hero, 2, hero->checkMirror());
      break;
    case 3:
      hero->repairHealth(30);
      break;
    case 4:
      fireShield += 1;
      break;
    case 5:
      fireUp += 10;
      break;
    case 6:
      fireDown += 10;
      break;
    case 7:
      simpleBlast += 10;
      break;
    case 8:
      pushingLaser += 10;
      break;
    default:
      simpleBlast += 10;
      break;
  }
}

void Item::drawBox() {
  glPushMatrix();
  glTranslatef(Object::getCenterX(), Object::getCenterY(), 0);
  Sprite::drawTile(0,0);
  glPopMatrix();
}

Item::~Item(){
#ifdef USE_SOUND
  //fmod_playsound(powerup);
#endif
}
