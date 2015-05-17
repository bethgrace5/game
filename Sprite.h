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
    int row;
    int column;
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

    void drawTilePure(int x, int y);
    void drawTile(int row, int column);
    void drawTilePure(int atSet);
    void drawTile(int atSet);

    void drawingTile(int row, int column);

    void drawSequence();
    void replaceTexture(GLuint take);
    GLuint textureBox();

    int getClipHeight();
    int getClipWidth();
    void setMirror(bool take);
    bool checkMirror();

    //Function Aid
    unsigned char *buildAlphaData2(Ppmimage *img);
};

#endif
