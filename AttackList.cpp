#include "AttackList.h"

#include "Object.h"
#include "Sprite.h"
#include "chadD.h"
#include "Enemy.h"
#include "Player.h"
#include "sounds.h"
//=====================================================================
//  Attack_Editor
//=====================================================================
void attack_list::makeAttacks(){
  int id = 0;
  int width, height;

  //Fire Aura Effect And Movment
  id = 0; width = 50; height = 50;
  sprite_sheet[id].insert("./images/fireball.ppm", 5, 5);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(1);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(width,height,0,0);
  attacks[id].changeRate(15);
  attacks[id].setTimeBase(true);
  attacks[id].setCycleBase(false);
  attacks[id].setDuration(300);
  attacks[id].setDamage(1);
  attacks[id].setStickOn(true);
  attacks[id].setMoveWith(true);
  attacks[id].setCharges(200);
  attacks[id].setVelocityX(10); attacks[id].setVelocityY(5);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(beep);
  attacks[id].setSoundCollide(censorBeep);
  #endif

  //Big Laser Projectile Attack
  id = 1; width = 25; height = 25;
  sprite_sheet[id].insert("./images/arrow.ppm", 4, 1);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(0);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(width,height,0,0);
  attacks[id].changeRate(15);
  attacks[id].setVelocityX(7);
  attacks[id].setPushBack(true);
  attacks[id].setPushAway(true);
  attacks[id].setTimeBase(true);
  attacks[id].setDuration(1000);
  attacks[id].setCycleBase(false);
  attacks[id].setCharges(30);
  attacks[id].setDamage(3);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(robotBlip1);
  attacks[id].setSoundCollide(robotBlip2);
  #endif

  //Fire Aura
  id = 2; width = 50; height = 50;
  sprite_sheet[id].insert("./images/fireball.ppm", 5, 5);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(1);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(width,height,0,0);
  attacks[id].setVelocityX(0);
  attacks[id].setVelocityY(0);
  attacks[id].setTimeBase(true);
  attacks[id].setDuration(15000);
  attacks[id].setCycleBase(false);
  attacks[id].setStickOn(true);
  attacks[id].setInvincible(true);//NOTE <- !THIS WILL ONLY EFFECT HERO!
  //attacks[id].setInvisiblity(false);//works for hero only
  attacks[id].setPushAway(false);
  attacks[id].setPushBack(false);
  //attacks[id].setMoveWith(true);
  //attacks[id].setConstantEffect(true);
  attacks[id].setInfiniteCharges(true);
  //attacks[id].setCharges(200);
  attacks[id].setDamage(25);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(tick);
  attacks[id].setSoundCollide(click);
  #endif

  //Fire Aura With Movement Downwards
  id = 3; width = 125; height = 125;
  sprite_sheet[id].insert("./images/fireball.ppm", 5, 5);
  sprite_sheet[id].setSize(75,75);
  sprite_sheet[id].setBackground(1);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(75,75,0,0);
  attacks[id].changeRate(15);
  attacks[id].setTimeBase(true);
  attacks[id].setCycleBase(false);
  attacks[id].setDuration(300);
  attacks[id].setDamage(1);
  attacks[id].setStickOn(true);
  attacks[id].setMoveWith(true);
  attacks[id].setCharges(200);
  attacks[id].setVelocityX(10); attacks[id].setVelocityY(-5);
#ifdef USE_SOUND
  attacks[id].setAttackSound(beep);
  attacks[id].setSoundCollide(0);
#endif

  //Hero Skiping Forward
  id = 4; width = 44; height = 48;
  sprite_sheet[id].insert("./images/hero.ppm", 13, 1);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(0);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].setInvisiblity(true);
  attacks[id].init(width,height,0,0);
  attacks[id].changeRate(20);
  attacks[id].setTimeBase(true);
  attacks[id].setCycleBase(false);
  attacks[id].setDuration(300);
  attacks[id].setDamage(5);
  attacks[id].setStickOn(true);
  attacks[id].setMoveWith(true);
  attacks[id].setCharges(100);
  attacks[id].setVelocityX(14); attacks[id].setVelocityY(1);
#ifdef USE_SOUND
  attacks[id].setAttackSound(beep);
  attacks[id].setSoundCollide(0);
#endif

  //EXPLODE
  id = 5; width = 150; height = 150;
  sprite_sheet[id].insert("./images/bossExplode.ppm", 5, 3);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(0);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(width,height,300,500);
  attacks[id].setVelocityX(0);
  attacks[id].setVelocityY(0);
  attacks[id].setTimeBase(false);
  attacks[id].setCycleBase(true);
  //attacks[id].setDuration(15000);
  attacks[id].changeRate(35);
  //attacks[id].setStickOn(true);
  //attacks[id].setInvincible(true);//NOTE <- !THIS WILL ONLY EFFECT HERO!
  //attacks[id].setInvisiblity(false);//works for hero only
  //attacks[id].setPushAway(false);
  //attacks[id].setPushBack(false);
  //attacks[id].setMoveWith(true);
  //attacks[id].setConstantEffect(true);
  attacks[id].setCharges(1000);
  attacks[id].setDamage(15);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(tick);
  attacks[id].setSoundCollide(click);
  #endif

  id = 6; width = 100; height = 100;
  sprite_sheet[id].insert("./images/arrow2.ppm", 4, 1);
  sprite_sheet[id].setSize(width,height);
  sprite_sheet[id].setBackground(0);
  attacks[id].referenceTo(sprite_sheet[id], id);
  attacks[id].init(width,height,0,0);
  attacks[id].changeRate(15);
  attacks[id].setTimeBase(true);
  attacks[id].setCycleBase(false);
  attacks[id].setDuration(300);
  attacks[id].setDamage(1);
  attacks[id].setStickOn(true);
  attacks[id].setMoveWith(true);
  attacks[id].setCharges(200);
  attacks[id].setVelocityX(10); attacks[id].setVelocityY(0);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(beep);
  attacks[id].setSoundCollide(censorBeep);
  #endif

}
//=====================================================================
//  Attack_Copy
//=====================================================================
void attack_list::copyAttack(Object *caster, int tId){
  if(currents_length >= MAX_CURRENTS) return;

  currents[currents_length] = new Attack(attacks[tId]);
  currents[currents_length]->setID(currents_length);

  currents[currents_length]->targetAt(caster);
  currents[currents_length]->setCenter(caster->getCenterX(),
      caster->getCenterY());
#ifdef USE_SOUND
  //cout << currents[currents_length]->getAttackSound()<<endl;
  fmod_playsound(currents[currents_length]->getAttackSound());
#endif
  currents_length++;
}

void attack_list::copyAttack(Object *caster, int tId, bool mirror){
  if(currents_length >= MAX_CURRENTS) return;
  attack_list::copyAttack(caster, tId);
  if(mirror){
    currents[currents_length-1]->setVelocityX(
        -currents[currents_length-1]->getVelocityX());
  }
}

void attack_list::copyAttack(Player *caster, int tId, bool mirror){
  if(currents_length >= MAX_CURRENTS) return;
  attack_list::copyAttack(caster, tId);
  currents[currents_length-1]->setEffectEnemy(true);
  if(mirror){
    currents[currents_length-1]->setVelocityX(
        -(currents[currents_length-1]->getVelocityX()));
  }
}

void attack_list::copyAttack(Enemy *caster, int tId, bool mirror){
  if(currents_length >= MAX_CURRENTS) return;
  attack_list::copyAttack(caster, tId);
  currents[currents_length-1]->setEffectPlayer(true);
  if(mirror){
    currents[currents_length-1]->setVelocityX(
        -currents[currents_length-1]->getVelocityX());
  }
}

void attack_list::deleteAttack(int id){
  if (currents_length <= 0) return;

  currents_length--;
  delete currents[id];
  currents[id] = (currents[currents_length]);
  currents[currents_length]=NULL;
}

//=====================================================================
//  Attack Outside Influence
//=====================================================================

bool attack_list::detectAttack (Object *obj, Attack *targetAttack) {
  //Gets (Moving Object, Static Object)
  //Reture True if Moving Object Collides with Static Object
  if (obj->getRight() > targetAttack->getLeft() &&
      obj->getLeft()   < targetAttack->getRight() &&
      obj->getBottom() < targetAttack->getTop()  &&
      obj->getTop()    > targetAttack->getBottom()) {
#ifdef USE_SOUND
    //cout << targetAttack->getSoundCollide()<<endl;
    fmod_playsound(targetAttack->getSoundCollide());
#endif
    return true;
  }
  return false;
}

void attack_list::renderAttacks(int x, int y){
  for (int i=0;i<currents_length;i++) {
    currents[i]->autoState();
    currents[i]->autoSet();
    currents[i]->cycleAnimations();

    glPushMatrix();
    glTranslatef(- x, - y, 0);
    currents[i]->drawBox(
        sprite_sheet[currents[i]->checkSpriteID()]);

    glEnd(); glPopMatrix(); 
  }
}
