#include "ppm.h"
#include "Object.h"
#include "Sprite.h"
#define rnd()(float)rand() /(float)(RAND_MAX)
//=====================================================================
//  Enemies
//=====================================================================
//How To Use...
//
////Global
//  enemies_length;
//  enemies[];
//
class Enemy : public Sprite, public Object {

    private:
        int speed;
        int type; // 0: enemy 1, 1: enemy 2, 2: boss
        int fire_rate;

    public:
        int life;
        Enemy(int w, int h, Object *ground);
        void draw();

};

Enemy::Enemy(int w, int h, Object *ground) : Sprite(), Object (w, h, ground->getCenterX()+rnd() * ground->getWidth() * (rnd()>.5?-1:1), ground->getCenterY() + ground->getHeight() + h) {
    life=100;
    //return newEnemy;
}
//this will set up Boundaries of the object and set Sprite to 0/NULL

void Enemy::draw() {

    float w, h;
    glPushMatrix();
    glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
    w = Object::getWidth();
    h = Object::getHeight();
/*
    glBegin(GL_QUADS);
    glVertex2i(-w, -h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();
*/
    //This Will Draw The TileSet based on the Boundaries of Object
    //glPushMatrix();
    //glTranslatef(Object::getLeft(), Object::getCenterY(), 0); 

    Sprite::drawTile(Object::getIndex(), 0, w, h);
    glPopMatrix(); 
}
