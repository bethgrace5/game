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
class Platform2 : public Object {
  private:
    int lineSpaceX;
    int lineSpaceY;
    int spriteID;

  public:
    Platform2();
    void setupTile();
    void drawRow();
    void saveLineSpace();
    void drawRow(int x, int y);
    void backgroundSwitch();
    void setSprite(Sprite take);

    void setSpriteID(int take);
    int getSpriteID();

    Sprite *targetSprite;
};

Platform2::Platform2(): Object(260, 200, 350, 400) {
  targetSprite = new Sprite();
  //targetSprite->setBackground(1);
  //std::cout << "Is Platform First\n";
  lineSpaceX = lineSpaceY = 0;
}

void Platform2::backgroundSwitch(){
  targetSprite->setBackground(1);
}

void Platform2::setSprite(Sprite take){
  targetSprite = &take;
}

void Platform2::setSpriteID(int take){
  spriteID = take;
}

int Platform2::getSpriteID(){
  return spriteID;
}

void Platform2::setupTile() {
  //Setup Object to match with tile rows
  //The Object Will Allocate to fit with the tile
  int widthSize;
  int heightSize;

  //The Object will resize itself to fit with rows of tile. (by Downsizing)
  float addWidth  = (int)Object::getWidth() % (int)targetSprite->getClipWidth();
  float addHeight = (int)Object::getHeight() % (int)targetSprite->getClipHeight();

  widthSize = Object::getWidth() - addWidth; 
  heightSize = Object::getHeight() - addHeight; 

  Object::init(widthSize, heightSize, Object::getCenterX(), Object::getCenterY());  

  saveLineSpace();
}

void Platform2::saveLineSpace(){
  lineSpaceX = targetSprite->getClipWidth();
  lineSpaceY = targetSprite->getClipHeight();
}

void Platform2::drawRow(int x, int y) {
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
  for(int j = 0; j < Object::getHeight()/lineSpaceY; j++){
    if (j == 0) glTranslatef(0, -lineSpaceY , 0); 
    else        glTranslatef(0, -(lineSpaceY + lineSpaceY), 0); 

    glPushMatrix();
    for (int i = 0; i < Object::getWidth()/lineSpaceX; i++) {

      if (i == 0) glTranslatef(lineSpaceX,0 , 0); 
      else        glTranslatef(lineSpaceX + lineSpaceX ,0 , 0); 

      targetSprite->drawTile();
    }
    glPopMatrix();
  } 
  glPopMatrix(); 
}
