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

extern attack_list boxA;
extern Player *hero;

Item::Item() : Sprite(), Object(20, 20, 350, 350) {
  effect=3;
  end = 0;
}

void Item::causeEffect(Player *hero) {
  switch (effect) {
    case 1: 
      hero->repairHealth(25);
      break;
    case 2:
      boxA.copyAttack(hero, 2, hero->checkMirror());
      break;
    case 3:
      hero->setGunType(5);
      break;
    default:
      hero->repairHealth(20);
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
