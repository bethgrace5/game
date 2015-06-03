
#ifndef ITEM_H
#define ITEM_H

#include "Player.h"
#include "Enemy.h"
#include "chadD.h"
#include "AttackList.h"

class Item : public Sprite, public Object {
    private: 
      int effect;
      int end;
    public:
      Item();
      Item(int take);
      ~Item();
      void causeEffect(Player *hero);
      void drawBox();
};

#endif 

