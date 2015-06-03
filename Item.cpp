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
  //effect=3;
  effect = rand() % 20 + 1;
  end = 0;
}

void Item::causeEffect(Player *hero) {
  hero->setAmmo(10);
  switch (effect) {
    case 1: 
      hero->repairHealth(25);
      break;
    case 2:
      boxA.copyAttack(hero, 2, hero->checkMirror());
      break;
    case 3:
      hero->setGunType(0);
      break;
    case 4:
      hero->setGunType(1);
      break;
    case 5:
      hero->setGunType(2);
      break;
    case 6:
      hero->setGunType(3);
      break;
    default:
      hero->repairHealth(10);
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
