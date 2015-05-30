//Storage
//#include Attack
#define MAX_GROUNDS 100

//====================================================================
//Platform Storage
//====================================================================
struct data{
  Platform grounds[MAX_GROUNDS];
  int grounds_length;
  int heroAtX, heroAtY;
} storeIn, storeBack;
//====================================================================



//====================================================================
//  Attack Storage
//====================================================================
struct attack_list{ //This Is What The Storage Will Save
  Sprite  sprite_sheet[25];
  Attack attacks[25];
  int attacks_length;

  Attack *currents[15];
  int currents_length;

  void makeAttack(int w, int h, int x, int y);
  void copyAttack(int cId, int tId);
  void deleteAttack(int id);
} boxA;

//====================================================================
//  Attack Storage Functions
//====================================================================
void attack_list::deleteAttack(int id){
  if (currents_length <= 0) return;

  std::cout << " What is current Attack_length " << currents_length<< std::endl;
  if (id < 0) return;
  std::cout << " what is id" << id << std::endl;

  currents[id] = new Attack();
  delete currents[id];

  for (int i = id; i < currents_length-1; i++) {
    currents[i] = currents[i + 1];
    currents[i]->setID(currents[i]->getID()-1);
  }

  currents[currents_length-1] = new Attack();
  delete currents[currents_length-1];
  currents_length--;
}

//=====================================================================
//
//=====================================================================

void attack_list::copyAttack(int cId, int tId){
  currents[currents_length] = new Attack(attacks[0]);
  currents[currents_length]->setCenter(350,350);

  currents[currents_length]->setID(currents_length);

  currents_length++;
}

//=====================================================================
//
//=====================================================================

bool detectAttack (Object *obj, Attack *targetAttack) {
  //Gets (Moving Object, Static Object)
  //Reture True if Moving Object Collides with Static Object
  if (obj->getRight() > targetAttack->getLeft() &&
      obj->getLeft()   < targetAttack->getRight() &&
      obj->getBottom() < targetAttack->getTop()  &&
      obj->getTop()    > targetAttack->getBottom()) {
    std::cout << "Hiting\n";
    //targetAttack->causeEffect(hero);
    //deleteAttack(obj->getID());
    return true;
  }
  return false;
}

void renderAttacks(int x, int y){
  for (int i=0;i<boxA.currents_length;i++) {
    boxA.currents[i]->cycleAnimations();
    glPushMatrix();
    glTranslatef(- x, - y, 0);
    boxA.currents[i]->drawBox(boxA.sprite_sheet[0]);
    glEnd(); glPopMatrix(); 
  }
}
