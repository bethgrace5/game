
#include "Sprite.h"
#include "Player.h"
#include "Item.h"

Item::Item() : Sprite(), Object(20, 20, 350, 350) {effect=1;}
void Item::causeEffect(Player *hero) {
    switch (effect) {
	case 1: hero->setHealth(0);
            break;
    }
}

void Item::drawBox() {
    glPushMatrix();
    glTranslatef(Object::getCenterX(), Object::getCenterY(), 0);
    Sprite::drawTile(0,0);
    glPopMatrix();

}


