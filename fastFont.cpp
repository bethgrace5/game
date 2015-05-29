#include "Platform.h"
#include "ppm.h"
//Program: Homework 3 
//Author: Ted Pascua
//Purpose: 
//           Make a function so you can easily write down words 
//           base on a ppm imaage.
//
//           Note: Some of the functions and add stuff are someone elses
//           I will not take credit for them

//=================+
//  Directory      |
//=================+
//Global Access    |all the variables
//Setup            |setting up the variables for making fonts
//Core Function    |making fonts
//=================+

void initFastFont();
void drawFont(int atSet);
void writeWords(std::string words, float x, float y);
void getFont(char letter);

void writePixel(int p, float x, float y);
void drawPixel(int pix);

//=====================================================================
//  Global Access
//=====================================================================
unsigned char *buildAlphaData2(Ppmimage *img);
float clipX = 0.142857;//7 in a row
float clipY = 0.166666;//6 in a Collumn
//7 letters in a row, and a total of 6 rows
Ppmimage *alphabetImage = NULL;
GLuint alphabetTexture;
float alphabetHeight, alphabetWidth;

Ppmimage *pixelImage = NULL;
GLuint pixelTexture;
float pixelHeight, pixelWidth;

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

  pixelImage = ppm6GetImage("./images/backgroundPixels.ppm");
  glGenTextures(2, &pixelTexture);
  glBindTexture(GL_TEXTURE_2D, pixelTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  unsigned char *pixData = buildAlphaData2(pixelImage);
  w = pixelWidth  = pixelImage->width;
  h = pixelHeight = pixelImage->height;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixData);
  delete [] pixData;
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
void drawFont(int atSet){
  int atX = atSet, atY = 0;
  //This is use to set the Y and X to the right places of each letter
  //  -- Since there are 7 letters in each row. Make sure to reset 
  if(atSet < 7);//nothing happen at this set
  else if(atSet < 14){ atX = atSet - 7 ; atY = 1;}
  else if(atSet < 21){ atX = atSet - 14; atY = 2;}
  else if(atSet < 28){ atX = atSet - 21; atY = 3;}
  else if(atSet < 35){ atX = atSet - 28; atY = 4;}
  else if(atSet < 42){ atX = atSet - 35; atY = 5;}
  else if(atSet < 49){ atX = atSet - 42; atY = 6;}

  glPushMatrix();
  //glTranslatef(500, 300, 0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, alphabetTexture);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_LESS, 1.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  float w = alphabetWidth/32;
  float h = alphabetHeight/32;

  glTexCoord2f(atX*clipX        , (atY*clipY)+clipY) ; glVertex2i(-w,-h);
  glTexCoord2f(atX*clipX        ,  atY*clipY)        ; glVertex2i(-w,h);
  glTexCoord2f((atX*clipX)+clipX,  atY*clipY)        ; glVertex2i(w,h);
  glTexCoord2f((atX*clipX)+clipX, (atY*clipY)+clipY) ; glVertex2i(w,-h);
/*
  glTexCoord2f(0*clipX, 1.0f)        ; glVertex2i(-w,-h);
  glTexCoord2f(0*clipX, 0.0f)        ; glVertex2i(-w,h);
  glTexCoord2f((0*clipX)+clipX, 0.0f); glVertex2i(w,h);
  glTexCoord2f((0*clipX)+clipX, 1.0f); glVertex2i(w,-h);
*/
  glEnd(); glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
}

void drawPixel(int pix){

  glPushMatrix();
  //glTranslatef(500, 300, 0);
  glBindTexture(GL_TEXTURE_2D, pixelTexture);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_TEXTURE_2D);
  glAlphaFunc(GL_GREATER, 0.0f);
  //glAlphaFunc(GL_LESS, 1.0f);
  glColor4ub(255,255,255,255);
  glBegin(GL_QUADS);
  float w = pixelWidth/4;
  float h = pixelHeight/2;

  if (pix == 0) {
      // draw 0
      glTexCoord2f(0  , 1) ; glVertex2i(-w,-h);
      glTexCoord2f(0  , 0) ; glVertex2i(-w,h);
      glTexCoord2f(0.5, 0) ; glVertex2i(w,h);
      glTexCoord2f(0.5, 1) ; glVertex2i(w,-h);
  }
  else {

      // draw 1
      glTexCoord2f(0.5, 1) ; glVertex2i(-w,-h);
      glTexCoord2f(0.5, 0) ; glVertex2i(-w,h);
      glTexCoord2f(1 , 0)  ; glVertex2i(w,h);
      glTexCoord2f(1 , 1)  ; glVertex2i(w,-h);
  }

  glEnd(); glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
}

//This Functions Gets A single letter base on the letter its given 
//it wiill call writeFont(letter) with the corresponding letter.
void getFont(char letter){
  //putchar(toupper(letter));
  switch(toupper(letter)){
    case 'A': drawFont(0); break;
    case 'B': drawFont(1); break;
    case 'C': drawFont(2); break;
    case 'D': drawFont(3); break;
    case 'E': drawFont(4); break;
    case 'F': drawFont(5); break;
    case 'G': drawFont(6); break;
    case 'H': drawFont(7); break;
    case 'I': drawFont(8); break;
    case 'J': drawFont(9); break;
    case 'K': drawFont(10); break;
    case 'L': drawFont(11); break;
    case 'M': drawFont(12); break;
    case 'N': drawFont(13); break;
    case 'O': drawFont(14); break;
    case 'P': drawFont(15); break;
    case 'Q': drawFont(16); break;
    case 'R': drawFont(17); break;
    case 'S': drawFont(18); break;
    case 'T': drawFont(19); break;
    case 'U': drawFont(20); break;
    case 'V': drawFont(21); break;
    case 'W': drawFont(22); break;
    case 'X': drawFont(23); break;
    case 'Y': drawFont(24); break;
    case 'Z': drawFont(25); break;
    case '!': drawFont(26); break;
    case '?': drawFont(27); break;
    case '1': drawFont(28); break;
    case '2': drawFont(29); break;
    case '3': drawFont(30); break;
    case '4': drawFont(31); break;
    case '5': drawFont(32); break;
    case '6': drawFont(33); break;
    case '7': drawFont(34); break;
    case '8': drawFont(35); break;
    case '9': drawFont(36); break;
    case '0': drawFont(37); break;
    case '+': drawFont(38); break;
    case '-': drawFont(39); break;
    case 'x': drawFont(40); break;
    case '/': drawFont(41); break; 
    default: break;
  }
}

//This Functions will get each letter of the words then calls getFont(letter)
//so.. this reads in every letter of the word. Each letter will have space
//between...
void writeWords(std::string words, float x, float y){

  int size = words.size(); 
  char cWords[size];
  strcpy(cWords, words.c_str());
  glPushMatrix();

  glTranslatef(x, y, 0);
  //glTranslatef(500, 300, 0);

  for(int i = 0; i < size; i++){
    glTranslatef(22, 0, 0);
    getFont(cWords[i]);
  }//Note Need To Add Spacing Between Each Letter
  glPopMatrix();

}
void writePixel(int p, float x, float y){

  glPushMatrix();
  glTranslatef(x, y, 0);
  glTranslatef(20, 0, 0);
  drawPixel(p);
  glPopMatrix();

}
