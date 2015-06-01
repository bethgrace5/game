//Storage

//====================================================================
//  Attack Storage
//====================================================================
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
  void copyAttack(int tId);
  void deleteAttack(int id);
} boxA;

//=====================================================================
//  Attack Storage Functions
//=====================================================================
void attack_list::deleteAttack(int id){
  if (currents_length <= 0) return;

  std::cout << " What is current Attack_length " << currents_length<< std::endl;
  if (id < 0) return;
  std::cout << " what is id" << id << std::endl;

  currents[id] = new Attack();
  //delete currents[id];

  for (int i = id; i < currents_length-1; i++) {
    currents[i] = currents[i + 1];
    currents[i]->setID(currents[i]->getID()-1);
  }

  currents[currents_length-1] = new Attack();
  //delete currents[currents_length-1];
  currents_length--;
}

void attack_list::copyAttack(int tId){
  if(currents_length >= MAX_CURRENTS) return;

  currents[currents_length] = new Attack(attacks[tId]);
  currents[currents_length]->setCenter(hero->getCenterX(),
      hero->getCenterY());
  currents[currents_length]->setID(currents_length);

  currents_length++;
}

void attack_list::makeAttacks(){
  boxA.sprite_sheet[0].insert("./images/fireball.ppm", 5, 5);
  boxA.sprite_sheet[0].setSize(50,50);
  boxA.sprite_sheet[0].setBackground(1);
  attacks[0].referenceTo(boxA.sprite_sheet[0], 0);
  attacks[0].init(50,50,0,0);
  attacks[0].changeRate(35);
  attacks[0].setVelocityX(10); attacks[0].setVelocityY(0);

  //Duration Base
  boxA.sprite_sheet[1].insert("./images/fireball.ppm", 5, 5);
  boxA.sprite_sheet[1].setSize(100,100);
  boxA.sprite_sheet[1].setBackground(1);
  attacks[1].init(50,50,0,0);
  attacks[1].setVelocityX(10);
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
    //targetAttack->causeEffect(hero);
    //deleteAttack(obj->getID());
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
    boxA.currents[i]->drawBox(boxA.sprite_sheet[0]);
    glEnd(); glPopMatrix(); 
  }
}

