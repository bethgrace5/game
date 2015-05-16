#include "ppm.h"
#include "Object.h"
#include "Sprite.h"
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
        void setupTile();
          void drawRow();
          void saveLineSpace();
          void drawRow(int x, int y);
          
};

Enemy::Enemy(int w, int h, Object *ground) : Sprite(), Object (w, h, ground->getCenterX(), ground->getCenterY() + ground->getHeight() + h) {
    //return newEnemy;
}
//this will set up Boundaries of the object and set Sprite to 0/NULL

   void Enemy::setupTile() {
//Setup Object to match with tile rows
//std::cout << "===================================================\n";
//std::cout << "Platform Size = " << Object::getWidth() << " , "
//  << Object::getHeight() << "\n"; 

//The Object Will Allocate to fit with the tile
int widthSize;
int heightSize;

//The Object will resize itself to fit with rows of tile. (by Downsizing)
float addWidth = (int)Object::getWidth() % (int)Sprite::getClipWidth();
float addHeight = (int)Object::getHeight() % (int)Sprite::getClipHeight();

widthSize = Object::getWidth() - addWidth; 
heightSize = Object::getHeight() - addHeight; 

Object::init(widthSize, heightSize, Object::getCenterX(), Object::getCenterY());  

//std::cout << "Clip Size = " << Sprite::getClipWidth() << " , " 
//  << Sprite::getClipHeight() << "\n";

//std::cout << "Platform Size = " << Object::getWidth() << " , "
//  << Object::getHeight() << "\n"; 

//std::cout << "Total tiles inside that fit side "
//  << Object::getWidth()/Sprite::getClipWidth()  << std::endl;

saveLineSpace();
}

void Enemy::drawRow() {
//Draws Tiles for a box
//This is To Test The Boundaries Of The Platform
//you will see a colored box

float w, h;
glPushMatrix();
glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
w = Object::getWidth();
h = Object::getHeight();

glBegin(GL_QUADS);
glVertex2i(-w, -h);
glVertex2i(-w, h);
glVertex2i( w, h);
glVertex2i( w,-h);
glEnd();
glPopMatrix();

//This Will Draw The TileSet based on the Boundaries of Object
glPushMatrix();
glTranslatef(Object::getLeft(), Object::getCenterY(), 0); 

for (int i = 0; i < Object::getWidth()/Sprite::getClipWidth(); i++) {

if (i == 0) glTranslatef(Sprite::getClipWidth(),0 , 0); 
else        glTranslatef(Sprite::getClipWidth() + Sprite::getClipWidth() ,0 , 0); 

Sprite::drawTile(0,0);
} 
glPopMatrix(); 
}

void Enemy::saveLineSpace(){
//lineSpaceX = Sprite::getClipWidth();
//lineSpaceY = Sprite::getClipHeight();
}

void Enemy::drawRow(int x, int y) {
//Draws Tiles for a box
//This is To Test The Boundaries Of The Platform
//you will see a colored box 
float w, h;
glPushMatrix();
glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
w = Object::getWidth();
h = Object::getHeight();

glBegin(GL_QUADS);
glVertex2i(-w, -h);
glVertex2i(-w, h);
glVertex2i( w, h);
glVertex2i( w,-h);
glEnd();
glPopMatrix();

//This Will Draw The TileSet based on the Boundaries of Object
glPushMatrix();
glTranslatef(Object::getLeft(), Object::getCenterY(), 0); 
/*
for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

    if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
    else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

    for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

        if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
        else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

        for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

            if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
            else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

            Sprite::drawTile(x,y);
        } 
        glPopMatrix(); 
    }
}*/
}

   /* ////////////////////////////////////////////////
    void Platform::drawColumn(int x, int y) {
        //Draws Tiles for a box
        //This is To Test The Boundaries Of The Platform
        //you will see a colored box 
        float w, h;
        glPushMatrix();
        glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
        w = Object::getWidth();
        h = Object::getHeight();

        glBegin(GL_QUADS);
        glVertex2i(-w, -h);
        glVertex2i(-w, h);
        glVertex2i( w, h);
        glVertex2i( w,-h);
        glEnd();
        glPopMatrix();

        //This Will Draw The TileSet based on the Boundaries of Object
        glPushMatrix();
        glTranslatef(Object::getLeft(), Object::getCenterY(), 0); 
        
    } */ 
