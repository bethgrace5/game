#ifndef PLATFORM_H
#define PLATFORM_H

//=====================================================================
//  Platform
//=====================================================================
//---------------------------------------------------------------------
class Platform : public Sprite, public Object{

  private:

  public:
    Platform();
    void setupTile();
    void drawRow();
};

Platform::Platform() : Sprite(), Object(260, 200, 350, 400){
    //Just An Intializer
    //this will set up Boundaries of the object and set Sprite to 0/NULL
}

void Platform::setupTile(){
  //Setup Object to match with tile rows
  std::cout << "===================================================\n";
  std::cout << "Platform Size = " << Object::getWidth() << " , "
    << Object::getHeight() << "\n"; 
  //The Object Will Allocate to fit with the tile
  int widthSize;
  int heightSize;

  //The Object will resize itself to fit with rows of tile. (by Downsizing)
  float addWidth = (int)Object::getWidth() % (int)Sprite::getClipWidth();

  widthSize = Object::getWidth() - addWidth; 
  heightSize = Sprite::getClipHeight();

  Object::init(widthSize, heightSize, Object::getCenterX(), Object::getCenterY());  

  std::cout << "Clip Size = " << Sprite::getClipWidth() << " , " 
    << Sprite::getClipHeight() << "\n";

  std::cout << "Platform Size = " << Object::getWidth() << " , "
    << Object::getHeight() << "\n"; 

  std::cout << "Total tiles inside that fit side "
    << Object::getWidth()/Sprite::getClipWidth()  << std::endl;
}

void Platform::drawRow(){
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
  for(int i = 0; i < Object::getWidth()/Sprite::getClipWidth(); i++){
    if(i == 0) glTranslatef(Sprite::getClipWidth(),0 , 0); 
    else       glTranslatef(Sprite::getClipWidth() + Sprite::getClipWidth() ,0 , 0); 
    Sprite::drawTile(1,1);
  }
  glPopMatrix(); 
}
#endif
