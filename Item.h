
#ifndef ITEM_H
#define ITEM_H

#include "Player.h"

class Item : public Sprite, public Object {
    private: 
      int effect;
      //int end;
    public:
      Item();
      void causeEffect(Player *hero);
      void drawBox();
};

#endif 

