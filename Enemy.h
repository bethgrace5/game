#ifndef ENEMY_H
#define ENEMY_H

#include "Object.h"
#include "Sprite.h"
#include "sounds.h"

//=====================================================================
//  Enemies
//=====================================================================
//How To Use...
//
////Global
//  enemies_length;
//  enemies[];
//

class Enemy : public Sprite, public Object {

    private:
        int speed; // X velocity speed
        //int type; // 1: enemy 1, 2: enemy 2, 3: boss
        int fire_rate; //ms between shooting
        int frame_rate; //ms between sprite indexes
        timeval fStart, fEnd; // frame timer
        timeval sStart, sEnd; // shooting timer

    public:
        int type; // 1: enemy 1, 2: enemy 2, 3: boss
        Enemy();
        Enemy(int w, int h, Object *ground, int t);
        Enemy(int w, int h, int x, int y, int t);
        void enemyAI(Object *hero);
        void draw();

};
#endif
