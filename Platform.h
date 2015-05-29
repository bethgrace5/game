
#ifndef PLATFORM_H
#define PLATFORM_H

//Chad Danner
//cs335
//HW3 
//05-11-15

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

#include "Sprite.h"
#include "Object.h"

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

#endif

