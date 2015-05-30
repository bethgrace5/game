#include <string.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glx.h>
#include <sys/time.h>
#include <iostream>
#include "ppm.h"
#include "sounds.h"
#include "Object.h"
#include "Sprite.h"
#include "definitions.h"
#include "Enemy.h"
using namespace std;

Enemy::Enemy(int w, int h, Object *ground, int t) : Sprite(), Object (w, h, ground->getCenterX()+rnd() * ground->getWidth() * (rnd()>.5?-1:1), ground->getCenterY() + ground->getHeight() + h) {
    type = t;
    switch (t){
        case 1: // enemy 1
            life=100;
            fire_rate=500;
            frame_rate=100;
            speed = 6;
            break;
        case 2: // enemy 2
            life=245;
            fire_rate=1200;
            frame_rate=150;
            speed = 1;
            break;
        case 3: // boss
            life=2000;
            fire_rate=2000;
            frame_rate=10;
            speed = 1;
            break;
    }
    gettimeofday(&fStart, NULL);
    gettimeofday(&sStart, NULL);
}

Enemy::Enemy(int w, int h, int x, int y, int t) : Sprite(), Object (w, h, x, y) {
    type = t;
    switch (t){
        case 1: // enemy 1
            life=100;
            fire_rate=500;
            frame_rate=100;
            speed = 6;
            break;
        case 2: // enemy 2
            life=245;
            fire_rate=1200;
            frame_rate=150;
            speed = 1;
            break;
        case 3: // boss
            life=2000;
            fire_rate=2000;
            frame_rate=10;
            speed = 1;
            break;
    }
    gettimeofday(&fStart, NULL);
    gettimeofday(&sStart, NULL);
}

void Enemy::enemyAI (Object *hero) {
    Object *e_f = Object::getFloor();
    Object *h_f = hero->getFloor();
    std::string str = "";
    if (!h_f){
        return;
    }
    str += "enemy " + itos(this) + " ";
    float h_cx=hero->getCenterX();
    //float h_cy=hero->getCenterY();
    //float h_vx=hero->getVelocityX();
    float h_fl=h_f->getLeft();
    float h_ft=h_f->getTop();
    float h_fr=h_f->getRight();
    //---------------------------------
    float e_cx=Object::getCenterX();
    float e_cy=Object::getCenterY();
    float e_vx=Object::getVelocityX();
    float e_vy=Object::getVelocityY();
    int e_t=type;

    Object::setOldCenter();
    Object::setCenter((e_cx + e_vx),(e_cy + e_vy));
    if (e_t!=3)
        Object::setVelocityY(e_vy - GRAVITY);
    else
        Object::setVelocityY(e_vy);

    if (type == 3){
        frame_rate++;
        if (frame_rate>=360)
            frame_rate=0;
    }

    float d_x=e_cx - hero->getCenterX();
    float d_y=e_cy - hero->getCenterY();
    int h_above = (d_y<0)?10:0;
    int h_right = (d_x<0)?1:0;
    int range=(Object::getAggro())?600:300;
    int rnd_tmp = (rnd()*100)+150;
        if (type==2)
            rnd_tmp=250;
        else if (type==3)
            rnd_tmp=500;
        int h_close = (((d_x*d_x)+(d_y*d_y)<(range*range) && !hero->isDying)?(((d_x*d_x)+(d_y*d_y)<(rnd_tmp*rnd_tmp))?2:1):0);
        // If enemy is within 200px & not dead: 2; within 400px: 1; else: 0
        int h_dir = h_above+h_right;
        if (!hero->isJumping and !hero->isFalling)
            h_dir+=((h_f==e_f)?100:0);
        str += "[DIR: " + itos(h_dir) + "] [" + itos(h_close) + "]";
        str += "{" + itos(Object::getAggro()) + "}";
        if ((type==1 or type==3) && h_close == 0 && Object::getAggro()){
            Object::setAggro(false);//Un-Aggro
            Object::setVelocityX(0);
            str += "Un-Aggro";
        }
        if (type == 3){
            if (h_close==2){
                //warp animation in center of boss
                //cout << "warming up..... . .. .. !!" << endl;
            }
        }
    if (e_f && h_f) { // If enemy is not mid jump
        float e_fl=e_f->getLeft();
        float e_ft=e_f->getTop();
        float e_fr=e_f->getRight();
        if (!(h_f==e_f) && (e_t == 2)){
            Object::setVelocityX(0);
            if (Sprite::getIndex()>5){
                Sprite::setIndex(0);
            }
            gettimeofday(&fEnd, NULL);
            if (diff_ms(fEnd, fStart)>frame_rate){
                Sprite::setIndex(((Sprite::getIndex()+1)%6));
                gettimeofday(&fStart, NULL);
            }
            return; // don't perform any AI on enemy 2 unless hero is on same ground object
        }
        switch (h_close) {
            case 2: // hero close range
                if (!Object::getAggro())
                    Object::setAggro(true);
                Object::setVelocityX(0);
                str += "attack!";
                //shoot
                gettimeofday(&sEnd, NULL);
                if (diff_ms(sEnd, sStart)>fire_rate){
                    if (type==1){
#ifdef USE_SOUND
                        //if (rnd()<.2)
                        fmod_playsound(laser); // laser
#endif
                        makeBullet(e_cx, e_cy+11, (h_right?17:-17), 8, 3);
                    } else if (type==2) {
#ifdef USE_SOUND
                        fmod_playsound(gunShotMarvin);
#endif
                        makeBullet(e_cx, e_cy+7, (h_right?7:-7), 20, 1);
                        Sprite::setIndex(6);
                        gettimeofday(&fStart, NULL);
                    } else if (type==3) {
                        gettimeofday(&sEnd, NULL);
                        if (diff_ms(sEnd, sStart)>fire_rate){
                            Object::isShooting=true;
                            gettimeofday(&fStart, NULL);
                        }
                        Object::setVelocityY(0);
                        Object::setVelocityX(0);
                    }
                    gettimeofday(&sStart, NULL);
                } else {
                    gettimeofday(&fEnd, NULL);
                    if (type==2 && diff_ms(fEnd, fStart)>frame_rate){
                        Sprite::setIndex(7);
                        gettimeofday(&fStart, NULL);
                    }
                }
                break;
            case 1: // follow hero
                if (!Object::getAggro())
                    Object::setAggro(true);
                str += "Aggro! ";
                switch (h_dir) {
                    case 0: // If hero is to the lower left of enemy
                        if (e_fl<e_cx) { // If enemy won't fall
                            Object::setVelocityX(speed*-1); // then move to the left
                            if (type==3)
                                Object::setVelocityY(speed*-1); // then move down
                            str += "move left!";
                        } else { // If enemy is going to fall
                            Object::setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fr>e_fl) {//grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<e_ft) {
                                    Object::setVelocityX(speed*-1);// Jump down
                                    str += "jump down!";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(1)";
                                }
                            }
                            else{ // gap
                                Object::setVelocityX(speed*-1);
                                str += "move left!";
                                Object::setFloor(NULL);
                            }
                        }
                        break;
                    case 1: // If hero is to the lower right of enemy
                        if (e_fr>e_cx) { // If enemy won't fall
                            Object::setVelocityX(speed); // then move to the right
                            if (type==3)
                                Object::setVelocityY(speed*-1); // then move down
                            str += "move right!";
                        } else { // If enemy is going to fall if he keeps going
                            Object::setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fl<=e_fr) { // grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<=e_ft) {
                                    Object::setVelocityX(speed);// Jump down
                                    str += "jump down!";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(2)";
                                }
                            }
                            else{ // gap
                                Object::setVelocityX(speed);
                                str += "move right!";
                                Object::setFloor(NULL);
                            }
                        }
                        break;
                    case 10: // If hero is to the upper left of enemy
                        //hero's ground is above enemy's ground
                        if (type==3)
                            Object::setVelocityY(speed); // then move up
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fr>e_fl) { // grounds are overlapping
                                if ((Object::getLeft())<h_fr) {
                                    Object::setVelocityX(speed);
                                    str += "move right";
                                }
                                else if ((Object::getLeft()-speed)>h_fr) {
                                    Object::setVelocityX(speed*-1);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityX(0);
                                    Object::setVelocityY(2);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the left with a gap
                                if ((Object::getLeft())>e_fl) {
                                    Object::setVelocityX(speed*-1);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityX(speed*-1);
                                    Object::setVelocityY(2);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the left
                            if ((Object::getLeft())>e_fl) {
                                Object::setVelocityX(speed*-1);//move to edge
                                str += "move left";
                            }
                            else if (h_f!=e_f) { // gap
                                Object::setVelocityX(0);
                                if ((e_fl-h_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(speed*-1);
                                    str += "move left!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fr>(e_fl-640)) {
                                    if ((h_cx+250)<e_cx) {
                                        Object::setVelocityX(speed*-1);// Jump over
                                    }
                                    Object::setVelocityY(2);
                                    str += "jump over!";
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 11: // If hero is to the upper right of enemy
                        //hero's ground is above enemy's ground
                        if (type==3)
                            Object::setVelocityY(speed); // then move up
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fl<e_fr) {
                                if ((Object::getRight())>h_fl) {
                                    Object::setVelocityX(speed*-1);
                                    str += "move left";
                                }
                                else if ((Object::getRight()+speed)<h_fl) {
                                    Object::setVelocityX(speed*-1);
                                    str += "move right";
                                }
                                else{
                                    Object::setVelocityX(0);
                                    str += "stop, ";
                                    Object::setVelocityY(2);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the right with a gap
                                if ((Object::getRight())<e_fr) {
                                    Object::setVelocityX(speed);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityX(speed);
                                    Object::setVelocityY(2);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the right
                            if ((Object::getRight())<e_fr) {
                                Object::setVelocityX(speed);//move to edge
                                str += "move right";
                            }
                            else if (h_f!=e_f) { // gap
                                Object::setVelocityX(0);
                                if ((h_fl-e_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(speed);
                                    str += "move right!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fl<(e_fr+640)) {
                                    if ((h_cx-250)>e_cx) {
                                        Object::setVelocityX(speed);// Jump over
                                    }
                                    else{
                                        Object::setVelocityX(((e_fr-h_fl)/59)*-1);// Jump over
                                        if (Object::getVelocityX()>speed)
                                            Object::setVelocityX(speed);
                                    }
                                    Object::setVelocityY(2);
                                    str += "jump over!";
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += " gap too large to jump!";
                                }
                            }
                        }
                        break;
                    case 100:
                    case 110:
                        if ((Object::getLeft())>e_fl) {
                            Object::setVelocityX(speed*-1);//move to edge
                            str += "move right";
                            if (type==2){
                                if (diff_ms(fEnd, fStart)>frame_rate){
                                    Sprite::setIndex(((Sprite::getIndex()+1)%16)+11);
                                    gettimeofday(&fStart, NULL);
                                }
                            }
                        }
                        else{
                            Object::setVelocityX(0);//move to edge
                        }
                        break;
                    case 101:
                    case 111:
                        if ((Object::getRight())<e_fr) {
                            Object::setVelocityX(speed);//move to edge
                            str += "move right";
                            if (type==2){
                                if (diff_ms(fEnd, fStart)>frame_rate){
                                    Sprite::setIndex(((Sprite::getIndex()+1)%16)+11);
                                    gettimeofday(&fStart, NULL);
                                }
                            }
                        }
                        else{
                            Object::setVelocityX(0);//move to edge
                        }
                        break;
                }
                break;
            case 0:
                // patrol
                Object::setAggro(false);
                if (e_vx==0) {
                    Object::setVelocityX((rnd()>.5)?(-0.6):(0.6));//Patrol ground object
                    if (type==3)
                        Object::setVelocityY(rnd()-0.5);//Boss floats around at random
                    str += "start patrolling";
                } else {
                    if (e_vx<0 && e_f!=NULL) {
                        if (Object::getLeft()<e_fl) {
                            e_vx*=-1; //must set this value for following code
                            Object::setVelocityX(e_vx);
                            str += "turn around";
                        }
                    } else if (e_vx>0 && e_f!=NULL) {
                        if (Object::getRight()>e_fr) {
                            e_vx*=-1; //must set this value for following code
                            Object::setVelocityX(e_vx);
                            str += "turn around";

                        }
                    }
                    if (type==3){
                        if (Object::getCenterY()>(h_cx+400)){
                            Object::setVelocityX(0);
                        }
                    }
                }
                break;
        }
        //Check for falling off
        if (Object::getFloor()) { // don't check e_f here
            if (e_vx>0 && e_cx > e_fr) {
                Object::setVelocityX(0);
                str += "almost fell!";
            }
            if (e_vx<0 && e_cx < e_fl) {
                Object::setVelocityX(0);
                str += "almost fell!";
            }
        }
    }
    else if (h_f && type==3) { // enemy is in the air
        if (Object::getBottom() < h_ft) { // enemy needs to go up
            if ((e_cx>h_fr) or (e_cx<h_fl)){
                str += "Enemy's bottom: " + itos(Object::getBottom()) + ", h_ft: " + itos(h_ft);
                Object::setVelocityY(speed);
                str += "; jump #" + itos(Object::getJump()) + " ";
            }
            else {
                if (h_cx<e_cx)
                    Object::setVelocityX(-speed);
                else
                    Object::setVelocityX(speed);
            }
        }
        else {
            Object::setVelocityY(-speed);
            if (e_cx<h_cx) {
                Object::setVelocityX(speed);
            }
            else{
                Object::setVelocityX(speed*-1);
            }

            str += "move in air, vel y: " + itos(Object::getVelocityY());
        }
    }
    else if (h_f && Object::getAggro()) { // enemy is in the air
        if (Object::getBottom() < h_ft) { // enemy needs to go up
            if ((e_cx>h_fr) or (e_cx<h_fl)){
                str += "Enemy's bottom: " + itos(Object::getBottom()) + ", h_ft: " + itos(h_ft);
                Object::setVelocityY(2);
                str += "; jump #" + itos(Object::getJump()) + " ";
            }
        }
        else {
            if (e_cx<h_cx) {
                Object::setVelocityX(speed);
            }
            else{
                Object::setVelocityX(speed*-1);
            }

            str += "move in air, vel y: " + itos(Object::getVelocityY());
        }
    }
    //std::cout << str << std::endl;
    if (Object::life<=0 && !(Object::isDying)){
        Object::isDying=1;
        if (type==1){
#ifdef USE_SOUND
            fmod_playsound(endFx);
#endif
            Sprite::setIndex(21);
        } else if (type==2) {
#ifdef USE_SOUND
            fmod_playsound(endFx);
#endif
            Sprite::setIndex(8);
        }
        gettimeofday(&fStart, NULL);
    }
    if (Object::isDying){
        gettimeofday(&fEnd, NULL);
        if (diff_ms(fEnd, fStart)>frame_rate){
            int end = 0;
            int tmp = (Sprite::getIndex()+1);
            if (type==1)
                end = 26;
            else if (type==2)
                end = 11;
            if (tmp<end){
                Sprite::setIndex(tmp);
            }
            else{
                Object::isDead=1;
            }
            gettimeofday(&fStart, NULL);
        }
    }
    else{ // normal walking indexes
        gettimeofday(&fEnd, NULL);
        if (type==1){
            if (diff_ms(fEnd, fStart)>frame_rate){
                Sprite::setIndex(((Sprite::getIndex()+1)%8)+(Object::getAggro()?8:0));
                gettimeofday(&fStart, NULL);
            }
        }
        else if (type==2){
          if (Object::getVelocityX()<0){
            Sprite::setMirror(true);
          } else {
            Sprite::setMirror(false);
          }
        }
    }
}

void Enemy::draw() {
    float w, h;
    glPushMatrix();
    glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
    if (type==3)
        glRotatef(frame_rate, 0, 0, Object::getCenterX());
    w = Object::getWidth();
    h = Object::getHeight();
    Sprite::drawTile(Sprite::getIndex(), 0, w, h);
    glPopMatrix(); 
}
