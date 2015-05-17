#include <iostream>
#include "functions.h"
#include "ppm.h"
#include "Object.h"
#include "Sprite.h"

//=====================================================================
//  Enemies
//=====================================================================
//How To Use...
//
////Global
//  enemies_length;
//  enemies[];
//
using namespace std;

class Enemy : public Sprite, public Object {

    private:
        int speed;
        int type; // 0: enemy 1, 1: enemy 2, 2: boss
        int fire_rate; //ms between shooting
        int frame_rate; //ms between sprite indexes
        timeval fStart, fEnd;
        timeval sStart, sEnd;

    public:
        int life;
        Enemy(int w, int h, Object *ground);
        void enemyAI(Object *hero);
        void draw();

};

Enemy::Enemy(int w, int h, Object *ground) : Sprite(), Object (w, h, ground->getCenterX()+rnd() * ground->getWidth() * (rnd()>.5?-1:1), ground->getCenterY() + ground->getHeight() + h) {
    life=100;
    fire_rate=220;
    frame_rate=100;
    gettimeofday(&fEnd, NULL);

}

void Enemy::enemyAI (Object *hero) {
    Object *e_f = Object::getFloor();
    Object *h_f = hero->getFloor();
    string str = "";
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

    Object::setOldCenter();
    Object::setCenter((e_cx + e_vx),(e_cy + e_vy));
    Object::setVelocityY(e_vy - GRAVITY);

    if (e_f && h_f) { // If enemy is not mid jump
        float e_fl=e_f->getLeft();
        float e_ft=e_f->getTop();
        float e_fr=e_f->getRight();
        float d_x=e_cx - hero->getCenterX();
        float d_y=e_cy - hero->getCenterY();
        int h_above = (d_y<0)?10:0;
        int h_right = (d_x<0)?1:0;
        int range=(Object::getAggro())?800:400;
        int rnd_tmp = (rnd()*100)+150;
        int h_close = (((d_x*d_x)+(d_y*d_y)<(range*range) && !hero->isDying)?(((d_x*d_x)+(d_y*d_y)<(rnd_tmp*rnd_tmp))?2:1):0);
        int h_dir = h_above+h_right;
        if (!hero->isJumping and !hero->isFalling)
            h_dir+=((h_f==e_f)?100:0);
        str += "[DIR: " + itos(h_dir) + "] [" + itos(h_close) + "]";
        str += "{" + itos(Object::getAggro()) + "}";
        if (h_close == 0 && Object::getAggro()){
            h_close=1;//Follow if aggro'd but over 800 pixels
        }
        // If enemy is within 200px & not dead: 2; within 400px: 1; else: 0
        switch (h_close) {
            case 2: // hero close range
                if (!Object::getAggro())
                    Object::setAggro(true);
                Object::setVelocityX(0);
                str += "attack!";
                //fall through?
                break;
            case 1: // follow hero
                if (!Object::getAggro())
                    Object::setAggro(true);
                str += "Aggro! ";
                switch (h_dir) {
                    case 0: // If hero is to the lower left of enemy
                        if (e_fl<e_cx) { // If enemy won't fall
                            Object::setVelocityX(-6); // then move to the left
                            str += "move left!";
                        } else { // If enemy is going to fall
                            Object::setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fr>e_fl) {//grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<e_ft) {
                                    Object::setVelocityX(-6);// Jump down
                                    str += "jump down!";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(1)";
                                }
                            }
                            else{ // gap
                                if ((e_fl-h_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(-6);
                                    str += "move left!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fr>(e_fl-640)) {
                                    if ((h_cx+250)<e_cx) {
                                        Object::setVelocityX(-6);// Jump over
                                    }
                                    else{
                                        Object::setVelocityX(((h_fr-e_fl)/59)*-1);// Jump over
                                        if (Object::getVelocityX()<-6)
                                            Object::setVelocityX(-6);
                                    }
                                    Object::setVelocityY(6);
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
                    case 1: // If hero is to the lower right of enemy
                        if (e_fr>e_cx) { // If enemy won't fall
                            Object::setVelocityX(6); // then move to the right
                            str += "move right!";
                        } else { // If enemy is going to fall if he keeps going
                            Object::setVelocityX(0); // then stop moving
                            str += "stop!";
                            if (h_fl<=e_fr) { // grounds are overlapping
                                //hero's ground is below enemy's ground
                                if (h_ft<=e_ft) {
                                    Object::setVelocityX(6);// Jump down
                                    str += "jump down!";
                                    Object::setFloor(NULL);
                                }
                                else{
                                    str += "NEVER CONDITION(2)";
                                }
                            }
                            else{ // gap
                                if ((h_fl-e_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(6);
                                    str += "move right!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fl<(e_fr+640)) {
                                    if ((h_cx-250)>e_cx) {
                                        Object::setVelocityX(6);// Jump over
                                    }
                                    else{
                                        Object::setVelocityX(((e_fr-h_fl)/59)*-1);// Jump over
                                        if (Object::getVelocityX()>6)
                                            Object::setVelocityX(6);
                                    }
                                    Object::setVelocityY(6);
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
                    case 10: // If hero is to the upper left of enemy
                        //hero's ground is above enemy's ground
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fr>e_fl) { // grounds are overlapping
                                if ((Object::getLeft()-6)<h_fr) {
                                    Object::setVelocityX(6);
                                    str += "move right";
                                }
                                else if ((Object::getLeft()-14)>h_fr) {
                                    Object::setVelocityX(-6);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityX(0);
                                    Object::setVelocityY(7);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the left with a gap
                                if ((Object::getLeft())>e_fl) {
                                    Object::setVelocityX(-6);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityY(7);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the left
                            if ((Object::getLeft())>e_fl) {
                                Object::setVelocityX(-6);//move to edge
                                str += "move left";
                            }
                            else if (h_f!=e_f) { // gap
                                Object::setVelocityX(0);
                                if ((e_fl-h_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(-6);
                                    str += "move left!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fr>(e_fl-640)) {
                                    if ((h_cx+250)<e_cx) {
                                        Object::setVelocityX(-6);// Jump over
                                    }
                                    else{
                                        Object::setVelocityX(((h_fr-e_fl)/59)*-1);// Jump over
                                        if (Object::getVelocityX()<-6)
                                            Object::setVelocityX(-6);
                                    }
                                    Object::setVelocityY(6);
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
                        if (h_ft>e_ft && h_ft<(e_ft+220)) {
                            if (h_fl<e_fr) {
                                if ((Object::getRight()+6)>h_fl) {
                                    Object::setVelocityX(-6);
                                    str += "move left";
                                }
                                else if ((Object::getRight()+14)<h_fl) {
                                    Object::setVelocityX(6);
                                    str += "move right";
                                }
                                else{
                                    Object::setVelocityX(0);
                                    str += "stop, ";
                                    Object::setVelocityY(7);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }
                            }
                            else{
                                //hero's floor is above and to the right with a gap
                                if ((Object::getRight())<e_fr) {
                                    Object::setVelocityX(6);
                                    str += "move left";
                                }
                                else{
                                    Object::setVelocityY(7);
                                    Object::setJump();
                                    str += "jump #" + itos(Object::getJump()) + " ";
                                    Object::setFloor(NULL);
                                }


                            }
                        }
                        else{
                            //hero's floor is equal or lower and to the right
                            if ((Object::getRight())<e_fr) {
                                Object::setVelocityX(6);//move to edge
                                str += "move right";
                            }
                            else if (h_f!=e_f) { // gap
                                Object::setVelocityX(0);
                                if ((h_fl-e_fr)<(e_ft-h_ft)) { // just run off
                                    Object::setVelocityX(6);
                                    str += "move right!";
                                    Object::setFloor(NULL);
                                }
                                else if (h_fl<(e_fr+640)) {
                                    if ((h_cx-250)>e_cx) {
                                        Object::setVelocityX(6);// Jump over
                                    }
                                    else{
                                        Object::setVelocityX(((e_fr-h_fl)/59)*-1);// Jump over
                                        if (Object::getVelocityX()>6)
                                            Object::setVelocityX(6);
                                    }
                                    Object::setVelocityY(6);
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
                        Object::setVelocityX(-6);
                        str += "move left!";
                        break;
                    case 101:
                    case 111:
                        Object::setVelocityX(6);
                        str += "move right!";
                        break;
                }
                break;
            case 0:
            // patrol
                Object::setAggro(false);
                if (e_vx==0) {
                    Object::setVelocityX((rnd()>.5)?(-0.6):(0.6));//Patrol ground object
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
    else if (h_f && Object::getAggro()) { // enemy is mid jump
        if (e_vx==0) {
            if ((e_vy > -0.5) &&
                    (e_vy <= 0)) {
                if (Object::getBottom() < h_ft) { // enemy needs to double jump
                    if (Object::getJump()<2) {
                        Object::setVelocityY(7);
                        Object::setJump();
                        str += "jump #" + itos(Object::getJump()) + " ";
                    }
                }
                else if (Object::getBottom() > h_ft) {
                    if (e_cx<h_cx) {
                        if (e_cx>(h_cx-100)) {
                            Object::setVelocityX(1);
                        }
                        else if (e_cx>(h_cx-250)) {
                            Object::setVelocityX(3);// Jump over
                        }
                        else{
                            Object::setVelocityX(6);
                        }
                    }
                    else{
                        if (e_cx<(h_cx+100)) {
                            Object::setVelocityX(-1);
                        }
                        else if (e_cx<(h_cx+250)) {
                            Object::setVelocityX(-3);// Jump over
                        }
                        else{
                            Object::setVelocityX(-6);
                        }
                    }
                    str += "move";
                }
            }
        }
        else{ //enemy is moving left or right
            e_vx=Object::getVelocityX();
            e_vy=Object::getVelocityY();
            if (e_cx>h_cx && e_vx>0) {
                Object::setVelocityX(0);
            }
            else if (e_cx<h_cx && e_vx<0) {
                Object::setVelocityX(0);
            }
            if ((e_vy > -0.5) && (e_vy <= 0)) { // check if enemy needs to double jump
                if (((e_vx > 0) && (e_cy < (h_ft+360)) && (e_cx < (h_fl-180))) or
                        ((e_vx < 0) && (e_cy > (h_ft+360)) && (e_cx > (h_fr+180)))) {
                    // enemy double jump to lower ground object
                    if (Object::getJump()<2) {
                        Object::setVelocityY(7);
                        Object::setJump();
                        str += "jump #" + itos(Object::getJump()) + " ";
                    }
                }
                else{
                    str += "too far? my x,y:" + itos(e_cx) + "," + itos(e_cy) + ";" + "ground's x,y:";
                    if (e_vx>0)
                        str += itos(h_fl) + "," + itos(h_ft);
                    else
                        str += itos(h_fr) + "," + itos(h_ft);
                }
            }
        }
    }
    cout << str << endl;
}

void Enemy::draw() {
    gettimeofday(&fEnd, NULL);
    if (diff_ms(fEnd, fStart)>frame_rate){
        Sprite::setIndex(((Sprite::getIndex()+1)%8)+(Object::getAggro()?8:0));
        gettimeofday(&fStart, NULL);
    }

    float w, h;
    glPushMatrix();
    glTranslatef(Object::getCenterX(), Object::getCenterY(), 0); 
    w = Object::getWidth();
    h = Object::getHeight();
    Sprite::drawTile(Sprite::getIndex(), 0, w, h);
    glPopMatrix(); 
}
