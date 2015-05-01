#ifndef FASTFONTS_H
#define FASTFONTS_H
//=================+
//  Directory      |
//=================+
//Global Access    |all the variables
//Setup            |setting up the variables for making fonts
//Core Function    |making fonts
//=================+

//Purpose: Get the alphabets 
/*
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <GL/glx.h>
*/

//#include "ppm.h"

//=====================================================================
//  Global Access
//=====================================================================
unsigned char *buildAlphaData2(Ppmimage *img);
float fontXdistance = 0.142857;
float fontYdistance = 0.166666;;

Ppmimage *alphabetImage = NULL;
GLuint alphabetTexture;

float alphabetHeight, alphabetWidth;
//=====================================================================
//  Setup
//=====================================================================
void initFastFont(){
  alphabetImage = ppm6GetImage("./images/DigitFont2.ppm");
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
//=====================================================================
//  Core Functions
//=====================================================================
void drawWords(){
  glPushMatrix();
  glTranslatef(500, 300, 0);
  glBindTexture(GL_TEXTURE_2D, alphabetTexture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_LESS, 1.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  float w = alphabetWidth/32;
  float h = alphabetHeight/32;
  float clipX = fontXdistance;
  float clipY = fontYdistance;

  glTexCoord2f(0*clipX        , (0*clipY)+clipY); glVertex2i(-w,-h);
  glTexCoord2f(0*clipX        , 0*clipY)        ; glVertex2i(-w,h);
  glTexCoord2f((0*clipX)+clipX, 0*clipY)        ; glVertex2i(w,h);
  glTexCoord2f((0*clipX)+clipX, (0*clipY)+clipY); glVertex2i(w,-h);
/*
  glTexCoord2f(0*clipX, 1.0f)        ; glVertex2i(-w,-h);
  glTexCoord2f(0*clipX, 0.0f)        ; glVertex2i(-w,h);
  glTexCoord2f((0*clipX)+clipX, 0.0f); glVertex2i(w,h);
  glTexCoord2f((0*clipX)+clipX, 1.0f); glVertex2i(w,-h);
*/

  glEnd(); glPopMatrix();

  glDisable(GL_ALPHA_TEST);
}

//This Functions Gets A single letter base on the letter its given 
//it wiill call writeFont(letter) with the corresponding letter.
void getFont(char letter){
  switch(letter){
    case 'T': break;
    case 'P': break;
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

#endif
