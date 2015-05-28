#include "ppm.h"

#ifndef SPRITE_H
#define SPRITE_H
//=====================================================================
//Sprite
//=====================================================================
//Note: This is used to hold the images of ppm files
//      Example how to use:
//        Sprite var1;
//        ... 
//        In initOpengl(){ //When Initiaizing Stuff
//          ...
//          var1.insert("image.ppm", row, column);
//          ...
//        }
//        //In Rendering Function
//          ...
//          var1.drawTile(x,y)
//
//          // This will draw on a coordinates of the tile
//          // make sure to use glTranslatef to change its coordinates
//
//For this to work properly, the image sheet must have sprite Evenly Split
//between each other
//---------------------------------------------------------------------
class Sprite{
  //Global Access-------------------------------------------------------
  private:
    const char *imageName;
    char save[40];
    int column;
    int row;
    int imageHeight;
    int imageWidth;
    int clipHeight;
    int clipWidth;
    float clipX;
    float clipY;
    int index;
    bool mirror;
    bool background;

    Ppmimage *image;
    GLuint texture;
    int tileAt;
    int indexX, indexY;
    int indexAt;

  //Functions-----------------------------------------------------------
  public:
    //Setting Up 
    Sprite();
    void insert(const char *filename, int x, int y);
    void setFile(const char *filename);
    void initSprite();
    void reInitSprite();
    void setClip(int x, int y);
    void setSize(int x, int y);

    //Outter Functions
    void drawFont(int atSet);
    void drawTile(int row, int column);
    void drawTile(int row, int column, int w, int h);
    void drawTile(int atSet);
    void drawTile();
    void drawSequence();
    void replaceTexture(GLuint take);
    GLuint textureBox();

    int getClipHeight();
    int getClipWidth();

    int getIndex();
    void setIndex(int ind);

    int getIndexX();
    int getIndexY();
    int getIndexAt();
    int getRow();
    int getColumn();

    void setMirror(bool reflect);
    bool checkMirror();
    void setBackground(bool back);
    bool checkBackground();

    void setIndexXY(int x, int y);
    void setIndexAt(int ind);
    //Function Aid
    unsigned char *buildAlphaData2(Ppmimage *img);
};
#endif
