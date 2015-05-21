//Chad Danner
//cs335
//HW3 
//05-11-15

#include "ppm.h"
#include "Object.h"
#include "Sprite.h"

//=====================================================================
//  Platform
//=====================================================================
//How To Use...
//
////Global
//  Platform var1; //This Sets Up The Platform Varaible
//
//  within init_opengl function // in main.cpp
//    add
//    var1.insert("./images/"ppm file"", columns, rows);
//    var1.init(width, height, x-cord, y-cord);
//    var1.setupTile();
//
//  within the movement function
//    groundCollide(hero, &var1);
//
//  within the render function
//    var1.drawRow(0,0);
//
class Platform : public Sprite, public Object {

  private:
    int lineSpaceX;
    int lineSpaceY;

  public:
    Platform();
    void setupTile();
    void drawRow();
    void saveLineSpace();
    void drawRow(int x, int y);
};

Platform::Platform() : Sprite(), Object(260, 200, 350, 400) {
    //Just An Intializer
    //this will set up Boundaries of the object and set Sprite to 0/NULL
}

void Platform::setupTile() {
  //Setup Object to match with tile rows
  //The Object Will Allocate to fit with the tile
  int widthSize;
  int heightSize;

  //The Object will resize itself to fit with rows of tile. (by Downsizing)
  float addWidth = (int)Object::getWidth() % (int)Sprite::getClipWidth();
  float addHeight = (int)Object::getHeight() % (int)Sprite::getClipHeight();

  widthSize = Object::getWidth() - addWidth; 
  heightSize = Object::getHeight() - addHeight; 

  Object::init(widthSize, heightSize, Object::getCenterX(), Object::getCenterY());  

  saveLineSpace();
}

void Platform::saveLineSpace(){
  lineSpaceX = Sprite::getClipWidth();
  lineSpaceY = Sprite::getClipHeight();
}

void Platform::drawRow(int x, int y) {
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
  glTranslatef(Object::getLeft(), Object::getTop(), 0);
  //glTranslatef(Object::getLeft(), 0, 0); 
  for(int j = 0; j < Object::getHeight()/lineSpaceY; j++){
    if (j == 0) glTranslatef(0, -lineSpaceY , 0); 
    else        glTranslatef(0, -(lineSpaceY + lineSpaceY), 0); 

    glPushMatrix();
    for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

      if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
      else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

      Sprite::drawTile(x,y);
    }
    glPopMatrix();
  } 
  glPopMatrix(); 
}

/*
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

for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

Sprite::drawTile(x,y);
} 
glPopMatrix(); 
}
*/