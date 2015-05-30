
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstring>
#include <sys/time.h>
#include <sstream>
#include <string>
#include "ppm.h"
#include "definitions.h"

using namespace std;

void makeBullet(int x, int y, int v_x, int dmg, int t);
extern int bullets;
typedef double Vec[3];

template <typename T> string itos (T Number) {
    stringstream ss;
    ss << Number;
    return (ss.str());
}
unsigned char *buildAlphaData (Ppmimage *img);
int diff_ms (timeval t1, timeval t2);

struct Bullet {
    int type; //0 = boss, 1 = enemy 2, 2 = hero, 3 = enemy 1
    int damage;
    Vec pos;
    Vec vel;
    float color[3];
    struct timespec time;
    struct Bullet *prev;
    struct Bullet *next;
    Bullet() {
        prev = NULL;
        next = NULL;
    }
};

extern Bullet *bulletHead;

//void makeBullet(int x, int y, int v_x, int t);

#endif
