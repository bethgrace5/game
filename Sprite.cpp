#include "Sprite.h"

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

  setClip(0, 0);
  imageHeight = 0;
  imageWidth = 0;
  tileAt = 0;
  mirror = 0;
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
  column = x; row = y; 
  std::cout << "What is (x,y): (" << column << "," << row << ")\n";
  if(x > 0) clipX = (float)1/column;
  std::cout << "give me CLIPX: " << clipX << std::endl;
  if(y > 0) clipY = (float)1/row;
  std::cout << "give me CLIPY: " << clipY << std::endl;
}

void Sprite::setSize(int x, int y){
  //Sets the size of sprite shown
  clipWidth = x;
  clipHeight = y;
  std::cout << "what is x " << x << "\n what is y " << y << std::endl;
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
  unsigned char *silhouetteData = buildAlphaData2(image);
  imageWidth  = image->width;
  imageHeight = image->height;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
  delete [] silhouetteData;
}

void Sprite::replaceTexture(GLuint take){
  //It replaces the texture with the texture it got
  //... but this function is not well defined yet
  texture = take;
}

void Sprite::setMirror(bool reflect){
  mirror = reflect;
}
bool Sprite::checkMirror(){
  return mirror; 
}
//=====================================================================
//Outter Functions
//=====================================================================
//  These are functions that are use to call drawing events using the variables
//  within this class. Also, giving out this class information
//  NOTE: drawTilePure and drawTile have diffrent Alpha Configuration
//  
//    Right Now I Think im going to make it use a booleon to check 
//    instead of having 4 diffrent functions


//These functions  Draws a tile base on line # base on below example
// 1  2  3  4  5  6  7
// 8  9  10 11 12 13 14

void Sprite::drawTile(int atSet){
  int atX = atSet;
  int atY = 0;
  atX = atSet % column;
  atY = (int)(atSet/column);
  Sprite::drawTile(atX, atY);
}

//These functions  Draws a tile base on row number and column number
//example:
//     1 2 3 4
//   1 # # # #
//   2 # # # #
//   3 # # # #

//This Draws Where you want to draw.
void Sprite::drawTile(int atX, int atY){
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.0f);

  glColor4ub(255,255,255,255);

  glBegin(GL_QUADS);
  int w = clipWidth;
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
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D);
}

//This will use the drawTile(int atSet)
//Continously calling this function will switch to the next sprite in the sheet
//Starting from the left then to right. It will do this for each row. Then reset
//back to the start.
void Sprite::drawSequence(){
  drawTile(tileAt); tileAt++;
  if(tileAt > row*column) tileAt = 0;
}

//==========================
GLuint Sprite::textureBox(){
  return texture;
}

int Sprite::getClipHeight(){
  return clipHeight;
}

int Sprite::getClipWidth(){
  return clipWidth;
}

int Sprite::getRow(){
  return row; 
}
int Sprite::getColumn(){
  return column;  
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
void Sprite::setIndexAt(int take ){
  indexAt = take; 
}
void Sprite::setIndexXY(int x, int y){
  indexX = x; indexY = y; 
}
//=====================================================================
//Function Aid
//=====================================================================
//Just A helper Function for the init() function in this group. 
//This gets the image convert them to computer readable bytes
unsigned char *Sprite::buildAlphaData2(Ppmimage *img){
  //add 4th component to RGB stream...
  int a,b,c;
  unsigned char *newdata, *ptr;
  unsigned char *data = (unsigned char *)img->data;
  //newdata = (unsigned char *)malloc(img->width * img->height * 4);
  newdata = new unsigned char[img->width * img->height * 4];
  ptr = newdata;
  for (int i=0; i<img->width * img->height * 3; i+=3) {
    a = *(data+0);
    b = *(data+1);
    c = *(data+2);
    *(ptr+0) = a;
    *(ptr+1) = b;
    *(ptr+2) = c;
    //get the alpha value
    *(ptr+3) = (a|b|c);
    ptr += 4;
    data += 3;
  }
  return newdata;
}
