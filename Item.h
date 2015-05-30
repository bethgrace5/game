
#ifndef ITEM_H
#define ITEM_H

#include "Player.h"

class Item : public Sprite, public Object {
    private: 
      int effect;
    public:
      Item();
      void causeEffect(Player *hero);
      void drawBox();
};

#endif 

