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
  effect = rand() % 10 + 1;
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
      boxA.copyAttack(hero, 2, 0);
      hero->setAmmo(20);
      //boxA.copyAttack(hero, 2, hero->checkMirror());
      break;
    case 3:
      hero->repairHealth(30);
      hero->setGunType(0);
      hero->setCoolDown(100);
      hero->setAmmo(3);
      break;
    case 4:
      hero->repairHealth(30);
      hero->setGunType(1);
      hero->setCoolDown(300);
      hero->setAmmo(5);
      break;
    case 5:
      hero->repairHealth(35);
      boxA.copyAttack(hero, 2, 0);
      break;
    case 6:
      hero->repairHealth(100);
      hero->setCoolDown(100);
      hero->setGunType(a_fireDown);
      break;
    case 7:
      hero->setGunType(a_simpleBlast);
      hero->setCoolDown(50);
      hero->setAmmo(100);
      hero->setCoolDown(5);
    case 8:
      hero->setGunType(a_pushingLaser);
      if(rand() % 100 + 1 <= 5){
        hero->setCoolDown(1);
        hero->setAmmo(9001); 
      }else{
        hero->setCoolDown(100);
        hero->setAmmo(3);  
      }
    default:
      hero->setGunType(a_simpleBlast);
      hero->setAmmo(20);
      hero->setCoolDown(50);
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

void Item::drawIndication(int i) {
  glPushMatrix();
  //Object::setCenter( 200, 200 );
  glTranslatef(WINDOW_HALF_WIDTH-220 + 35*i, WINDOW_HEIGHT-30, 0);
  Sprite::drawTile(0,0);
  glPopMatrix();
}

int Item::getEffect() {
    return effect;
}

Item::~Item(){
#ifdef USE_SOUND
  //fmod_playsound(powerup);
#endif
}
