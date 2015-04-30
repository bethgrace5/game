#ifndef FASTFONTS_H
#define FASTFONTS_H
/*
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <GL/glx.h>
*/

#include "ppm.h"

unsigned char *buildAlphaData2(Ppmimage *img);

typedef float VecPos[3]; //Has 3 Positions X, Y, Z. its like the Vec but its 
                         //in an array. [x, y, z]

//These Are Coordinat make up the meshes for a letter, 
//These Letters are made up of variety of triangles.
//Multiple Triangles are needed tomake these letters
//
//  For Each 3 Brackets, {x, y, z}, marks the corners of a signle triangle.
//    i tried sperate 3 {} from the other 3{}
//    by puting them from enough position from the others, 
//    to make it easier to read.

VecPos letterT[18] = {//Letter T
  {1,7,0}, {1,5,0}, {3,5,0}      ,   {1,7,0}, {3,5,0}, {5,5,0},
  {1,7,0}, {5,5,0}, {7,7,0}      ,   {5,5,0}, {7,7,0}, {7,5,0},
  {3,5,0}, {5,5,0}, {3,0,0}      ,   {3,0,0}, {5,0,0}, {5,5,0}
}; int ntParts = 18;

VecPos letterP[33] = {//Letter P
  {1,0,0}, {3,0,0}, {3,3,0}  ,  {1,0,0}, {3,3,0}, {3,4,0},
  {1,0,0}, {3,4,0}, {3,6,0}  ,  {1,0,0}, {1,7,0}, {3,6,0},
  {1,7,0}, {3,6,0}, {4,7,0}  ,  {3,6,0}, {4,7,0}, {5,6,0},
  {3,6,0}, {4,5,0}, {5,6,0}  ,  {4,5,0}, {5,6,0}, {5,4,0},
  {4,5,0}, {5,4,0}, {4,3,0}  ,  {4,5,0}, {4,3,0}, {3,4,0},
  {3,3,0}, {3,4,0}, {4,3,0}
}; int npParts = 33;

float fontXdistance = 0.11111;
float fontYdistance = 0.33333;

Ppmimage *alphabetImage = NULL;
GLuint alphabetTexture;

float alphabetHeight, alphabetWidth;

void initFastFont(){
  alphabetImage = ppm6GetImage("./images/Alphabets.ppm");
  glGenTextures(2, &alphabetTexture);
  glBindTexture(GL_TEXTURE_2D, alphabetTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  unsigned char *silhouetteData = buildAlphaData2(alphabetImage);
  int w = alphabetWidth  = alphabetImage->width;
  int h = alphabetHeight = alphabetImage->height;
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, silhouetteData);
  delete [] silhouetteData;
}

void drawWords(){
  glPushMatrix();
  glTranslatef(500, 100, 0);
  glBindTexture(GL_TEXTURE_2D, alphabetTexture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_LESS, 1.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  float w = alphabetWidth/3;

  float tl_sz = 0.1171111;
  /*
    if ((sprite->getVelocityX() < 0.0) or (sprite->getOldCenterX()>sprite->getCenterX())){
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 1.0f); glVertex2i(-w,-w);
        glTexCoord2f((sprite->getIndex()*tl_sz)+tl_sz, 0.0f); glVertex2i(-w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz), 0.0f); glVertex2i(w,w);
        glTexCoord2f((sprite->getIndex()*tl_sz), 1.0f); glVertex2i(w,-w);
    } else {*/
        glTexCoord2f(0*tl_sz, 1.0f)        ; glVertex2i(-w,-w);
        glTexCoord2f(0*tl_sz, 0.0f)        ; glVertex2i(-w,w);
        glTexCoord2f((0*tl_sz)+tl_sz, 0.0f); glVertex2i(w,w);
        glTexCoord2f((0*tl_sz)+tl_sz, 1.0f); glVertex2i(w,-w);
    //}
    glEnd(); glPopMatrix();
    glDisable(GL_ALPHA_TEST);
}

//This is An Example of how to Write Down a single Font
void writeFont(){
  int check3 = 0; //Use to Check if all 3 points of the traingles are use

  //size is use Check how many coordiantes a letter have
  int size = (sizeof(letterT)/sizeof(*letterT));
  
  //This Will Draw A Traingle for every 3*i; 3 times the amount of iterations
  for(int i = 0; i < size; i++){
    if(check3 == 0) glBegin(GL_TRIANGLES);

    glVertex2i(letterT[i][0], letterT[i][1]); check3++;

    if(check3 == 3){
      check3 = 0; glEnd();
    }
  }
}

//  Flexible font writing.
//  -- this writeFont will take a VecPos Variable. And Uses its cordinates
//     to make a letter. THIS is almost the  same as the top.
void writeFont(VecPos letterTake[], int size){
  int check3 = 0;
   //Cant get size of a pointer to an array
  //int size = (sizeof(letterTake)/sizeof(*letterTake));
  //std::cout << "what is size : " << size << "\n";
  
  for(int i = 0; i < size; i++){
    if(check3 == 0) glBegin(GL_TRIANGLES);

    glVertex2i(letterTake[i][0], letterTake[i][1]); check3++;

    if(check3 == 3){check3 = 0; glEnd();}
  }
}

//This Functions Gets A single letter base on the letter its given 
//it wiill call writeFont(letter) with the corresponding letter.
void getFont(char letter){
  switch(letter){
    case 'T': writeFont(letterT, int( sizeof(letterT)/sizeof(*letterT) )); break;
    case 'P': writeFont(letterP, int( sizeof(letterP)/sizeof(*letterP) )); break;
    //case 'P': writeFont(letterP); break;
    default: break;
  }
}

//This Functions will get each letter of the words then calls getFont(letter)
//so.. this reads in every letter of the word. Each letter will have space
//between...
//not finish yet
void getWords(std::string words){
 int size = words.size(); 
 for(int i = 0; i < size; i++){
   //getFont(words.charAt(0));
 }//Note Need To Add Spacing Between Each Letter


}

unsigned char *buildAlphaData2(Ppmimage *img){
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
#endif
