//====================================================================
//  Attack Storage
//====================================================================
/*
-------------------
 Attack_Editor
 Attack_Usage
 Outside_Influence
-----------------
*/
extern Player *hero;
#define MAX_ATTACKS 25
#define MAX_CURRENTS 15

struct attack_list{
  Sprite  sprite_sheet[MAX_ATTACKS];
  Attack attacks[MAX_ATTACKS];
  int attacks_length;

  Attack *currents[MAX_CURRENTS];
  int currents_length;

  void makeAttacks();
  void copyAttack(Object *caster, int tId);
  void copyAttack(Object *caster, int tId, bool mirror);
  void copyAttack(Player *caster, int tId, bool mirror);
  void copyAttack(Enemy *caster, int tId, bool mirror);
  void deleteAttack(int id);

} boxA;
//=====================================================================
//  Attack_Editor
//=====================================================================
void attack_list::makeAttacks(){
  int id = 0;
  int width, height;
  //Aura Effect And Movment
  boxA.sprite_sheet[0].insert("./images/fireball.ppm", 5, 5);
  boxA.sprite_sheet[0].setSize(75,75);
  boxA.sprite_sheet[0].setBackground(1);
  attacks[0].referenceTo(boxA.sprite_sheet[0], 0);
  attacks[0].init(75,75,0,0);
  attacks[0].changeRate(15);
  attacks[0].setTimeBase(true);
  attacks[0].setCycleBase(false);
  attacks[0].setDuration(300);
  attacks[0].setDamage(1);
  //attacks[0].setStickOn(true);
  attacks[0].setMoveWith(true);
  attacks[0].setCharges(20);
  attacks[0].setVelocityX(10); attacks[0].setVelocityY(5);
  #ifdef USE_SOUND
  attacks[0].setAttackSound(beep);
  attacks[0].setSoundCollide(0);
  #endif

  //Projectile Attack
  boxA.sprite_sheet[1].insert("./images/fireball.ppm", 5, 5);
  boxA.sprite_sheet[1].setSize(25,25);
  boxA.sprite_sheet[1].setBackground(1);
  attacks[1].referenceTo(boxA.sprite_sheet[1], 1);
  attacks[1].init(25,25,0,0);
  attacks[1].setVelocityX(10);
  attacks[1].setTimeBase(true);
  attacks[1].setCycleBase(false);
  attacks[1].setCharges(10);
  attacks[1].setDamage(25);
  #ifdef USE_SOUND
  attacks[1].setAttackSound(beep);
  attacks[1].setSoundCollide(0);
  #endif


  //Template
  id = 2; width = 50; height = 50;
  boxA.sprite_sheet[id].insert("./images/fireball.ppm", 5, 5);
  boxA.sprite_sheet[id].setSize(width,height);
  boxA.sprite_sheet[id].setBackground(id);
  attacks[id].referenceTo(boxA.sprite_sheet[id], id);
  attacks[id].init(width,height,0,0);
  attacks[id].setVelocityX(0);
  attacks[id].setVelocityY(0);
  attacks[id].setTimeBase(true);
  attacks[id].setDuration(5000);
  attacks[id].setCycleBase(false);
  attacks[id].setStickOn(true);
  //attacks[id].setMoveWith(true);
  //attacks[id].setConstantEffect(true);
  attacks[id].setCharges(100);
  attacks[id].setDamage(25);
  #ifdef USE_SOUND
  attacks[id].setAttackSound(beep);
  attacks[id].setSoundCollide(0);
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
  //fmod_playsound(currents[currents_length]->getAttackSound());
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
bool detectAttack (Object *obj, Attack *targetAttack) {
  //Gets (Moving Object, Static Object)
  //Reture True if Moving Object Collides with Static Object
  if (obj->getRight() > targetAttack->getLeft() &&
      obj->getLeft()   < targetAttack->getRight() &&
      obj->getBottom() < targetAttack->getTop()  &&
      obj->getTop()    > targetAttack->getBottom()) {
      #ifdef USE_SOUND
       //fmod_playsound(currents[currents_length]->getSoundCollide());
      #endif
    return true;
  }
  return false;
}

void renderAttacks(int x, int y){
  for (int i=0;i<boxA.currents_length;i++) {
    boxA.currents[i]->autoState();
    boxA.currents[i]->autoSet();
    boxA.currents[i]->cycleAnimations();

    glPushMatrix();
    glTranslatef(- x, - y, 0);
    boxA.currents[i]->drawBox(
        boxA.sprite_sheet[boxA.currents[i]->checkSpriteID()]);

    glEnd(); glPopMatrix(); 
  }
}

