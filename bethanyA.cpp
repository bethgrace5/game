#include "Sprite.h"
#include "functions.h"

//====================================================================
//
//=====================================================================
//Setting Up
//=====================================================================
//    -- This sections includes all functions that changes and initialize
//       the variables in the sprite class.

//Default Contructor
Sprite::Sprite(){
  //when making a Sprite Variable, sets all variables within to 0 or NULL
  imageName = NULL;
  texture = 0;
  clipX = clipY = 0;
  clipWidth = clipHeight = 0;
  index = 0;
  setClip(0, 0);
  imageHeight = 0;
  imageWidth = 0;
  tileAt = 0;
}

void Sprite::setIndex(int ind){
    index = ind;
}

int Sprite::getIndex() {
    return index;
}

void Sprite::insert(const char *filename, int x, int y){
  //Gets the number of images place in a row and column x , y
  //then gets the name of the image file
  setFile(filename);
  initSprite(); 
  setClip(x, y); 
  setSize(imageWidth * clipX, imageHeight * clipY);
}

void Sprite::setFile(const char *filename){
  //Change the imageName to ppm file. imageName is used in the initSprite();
  imageName = filename; 
}

void Sprite::setClip(int x, int y){
  //User defined row and column. The images must evenly split apart from each
  //other. ClipX and ClipY determines where to cut parts in the image.
  row = x; column = y; 
  //std::cout << "What is (x,y): (" << row << "," << column << ")\n";
  if(x > 0) clipX = (float)1/row;
  //std::cout << "give me CLIPX: " << clipX << std::endl;
  if(y > 0) clipY = (float)1/column;
  //std::cout << "give me CLIPY: " << clipY << std::endl;
}

void Sprite::setSize(int x, int y){
  //Sets the size of sprite shown
  clipWidth = x;
  clipHeight = y;
  //std::cout << "what is x " << x << "\n what is y " << y << std::endl;
}

void Sprite::initSprite(){
  //Get an image in input inside the texure
  image = ppm6GetImage(imageName);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

 // glEnable (GL_BLEND);
 // glBlendFunc (GL_ONE, GL_ONE);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  unsigned char *silhouetteData = buildAlphaData(image);
  imageWidth  = image->width;
  imageHeight = image->height;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
  delete [] silhouetteData;
  strImageName = imageName;
}

void Sprite::reInitSprite(){
  //Refresh the new Sprite Image
  imageName = strImageName.c_str();
  image = ppm6GetImage(imageName);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  unsigned char *silhouetteData = buildAlphaData(image);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
  delete [] silhouetteData;
}

void Sprite::replaceTexture(GLuint take){
  //It replaces the texture with the texture it got
  //... but this function is not well defined yet
  texture = take;
}
//=====================================================================
//Outter Functions
//=====================================================================
//  These are functions that are use to call drawing events using the variables
//  within this class. Also, giving out this class information


//This Draws a tile base on row number and column number
//example:
//     1 2 3 4
//   1 # # # #
//   2 # # # #
//   3 # # # #
//
void Sprite::drawTile(int row, int column){
  //Need to check if 0;
  int atX = row; int atY = column;

  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  int  w = clipWidth;
  int h = clipHeight;
  if(!checkMirror()){
    glTexCoord2f(atX*clipX        , (atY*clipY)+clipY) ; glVertex2i(-w,-h);
    glTexCoord2f(atX*clipX        ,  atY*clipY)        ; glVertex2i(-w,h);
    glTexCoord2f((atX*clipX)+clipX,  atY*clipY)        ; glVertex2i(w,h);
    glTexCoord2f((atX*clipX)+clipX, (atY*clipY)+clipY) ; glVertex2i(w,-h);
  }else{
    glTexCoord2f((atX*clipX)+clipX, (atY*clipY)+clipY) ; glVertex2i(-w,-h);
    glTexCoord2f((atX*clipX)+clipX,  atY*clipY)        ; glVertex2i(-w,h);
    glTexCoord2f(atX*clipX        ,  atY*clipY)        ; glVertex2i(w,h);
    glTexCoord2f(atX*clipX        , (atY*clipY)+clipY) ; glVertex2i(w,-h);
  }
  glEnd(); glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
}

void Sprite::drawTile(int row, int column, int w, int h){
  //Need to check if 0;
  int atX = row; int atY = column;

  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);

  glTexCoord2f(atX*clipX        , (atY*clipY)+clipY) ; glVertex2i(-w,-h);
  glTexCoord2f(atX*clipX        ,  atY*clipY)        ; glVertex2i(-w,h);
  glTexCoord2f((atX*clipX)+clipX,  atY*clipY)        ; glVertex2i(w,h);
  glTexCoord2f((atX*clipX)+clipX, (atY*clipY)+clipY) ; glVertex2i(w,-h);

  glEnd(); glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
}

//This Draws a tile base on line # base on below example
// 1  2  3  4  5  6  7
// 8  9  10 11 12 13 14
//
void Sprite::drawTile(int atSet){
  int atX = atSet, atY = 0;         
  atX = atSet % row;
  atY = (int)(atSet/row);

  drawTile(atX, atY);
/*
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_LESS, 1.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  int w = clipWidth;
  int h = clipHeight;

  glTexCoord2f(atX*clipX        , (atY*clipY)+clipY) ; glVertex2i(-w,-h);
  glTexCoord2f(atX*clipX        ,  atY*clipY)        ; glVertex2i(-w,h);
  glTexCoord2f((atX*clipX)+clipX,  atY*clipY)        ; glVertex2i(w,h);
  glTexCoord2f((atX*clipX)+clipX, (atY*clipY)+clipY) ; glVertex2i(w,-h);

  glEnd(); glPopMatrix();
  glDisable(GL_ALPHA_TEST);*/
}

//This will use the drawTile(int atSet)
//Continously calling this function will switch to the next sprite in the sheet
//Starting from the left then to right. It will do this for each row. Then reset
//back to the start.
void Sprite::drawSequence(){
  drawTile(tileAt); tileAt++;
  if(tileAt > row*column) tileAt = 0;
}

//Gives out the texture data/value
GLuint Sprite::textureBox(){
  return texture;
}

int Sprite::getClipHeight(){
  return clipHeight;
}

int Sprite::getClipWidth(){
  return clipWidth;
}

int Sprite::getColumn(){
  return column;
}
int Sprite::getRow(){
  return row;
}
void Sprite::setMirror(bool reflect){
  mirror = reflect;
}
bool Sprite::checkMirror(){
  return mirror;
}

int Sprite::getIndexX(){
  return indexX;
}
int Sprite::getIndexY(){
  return indexY;
}
int Sprite::getIndexAt(){
  return indexAt;
}
void Sprite::setIndexAt(int ind){
  indexAt = ind; 
}
void Sprite::setIndexXY(int x, int y){
  indexX = x;
  indexY = y;
}

