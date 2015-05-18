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
    int column;
    int row;
    int imageHeight;
    int imageWidth;
    int clipHeight;
    int clipWidth;
    float clipX;
    float clipY;
    bool mirror;

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
    void setClip(int x, int y);
    void setSize(int x, int y);

    //Outter Functions
    //void drawFont(int atSet);

    void drawTile(int column, int row);
    void drawTile(int atSet);

    void drawSequence();
    void replaceTexture(GLuint take);
    GLuint textureBox();

    int getClipHeight();
    int getClipWidth();
    void setMirror(bool reflect);
    bool checkMirror();

    int getColumn();
    int getRow();
    int getIndexX();
    int getIndexY();
    int getIndexAt(); 

    void setIndexXY(int x, int y);
    void setIndexAt(int take);
    //Function Aid
    unsigned char *buildAlphaData2(Ppmimage *img);
};

#endif
