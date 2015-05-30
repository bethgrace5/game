
#include <sstream>
#include <iostream>
#include "ppm.h"
#include "functions.h"
using namespace std;

extern int level;

unsigned char *buildAlphaData (Ppmimage *img) {
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

int diff_ms (timeval t1, timeval t2) {
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}

void makeBullet(int x, int y, int v_x, int dmg, int t){
    //cout << "bullets: " << bullets << endl;
    Bullet *b = new Bullet;
    b->pos[0] = x;
    b->pos[1] = y;
    b->vel[0] = v_x;
    b->vel[1] = 0;
    b->type=t;
    b->damage=dmg;
    b->next = bulletHead;
    if (bulletHead != NULL)
        bulletHead->prev = b;
    bulletHead = b;
    bullets++;
    //cout << "created bullet, bullets: " << bullets << endl;
}

