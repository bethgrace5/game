// Class Object
// can be used to create AI sprites as well
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
<<<<<<< HEAD

class Object{
    struct Vec {
        float x, y, z;
    };
    public:
        void setWidth(double w);
        double getWidth(void);
        void setHeight(double w);
        double getHeight(void);

        double getCenterX(void);
        double getCenterY(void);

        double getVelocityX(void);
        double getVelocityY(void);

        double getCameraX(void);
        double getCameraY(void);

        void setCenter(double x, double y);
        void setCameraX(double x);
        void setCameraY(double y);

        void setVelocityX(double x);
        void setVelocityY(double y);

        double getOldCenterX(void);
        double getOldCenterY(void);
        void  setOldCenter(void);

        double getTop(void);
        double getRight(void);
        double getBottom(void);
        double getLeft(void);

        double getOldTop(void);
        double getOldRight(void);
        double getOldBottom(void);
        double getOldLeft(void);
 
        void setIndex(int ind);
        int getIndex();

        Object( double w, double h, double x, double y);
        // TODO: create destructor
        //~Object(){ delete [] ground;}

    private:
        int width;
        int height;
        int windowCenter;
        // index is for sprite image tiles
        int index;
        Vec camera;
        Vec velocity;
        Vec center;
        Vec oldCenter;
};
=======
#include "Object.h"
>>>>>>> 012bf37183c4fe9ce47174f07895c77f8c2e661a

Object::Object( double w, double h, double x, double y ) {
    width  = w;
    height = h;
    center.x = x;
    center.y = y;
    center.z = 0;
	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;
	camera.x = 0;
	camera.y = 0;
	camera.z = 0;
<<<<<<< HEAD
    index = 0;
=======
    windowCenter.x = 300;
    windowCenter.y = 300;
    windowCenter.z = 0;
    interval = 50;
>>>>>>> 012bf37183c4fe9ce47174f07895c77f8c2e661a
}
void Object::setWidth( double w ) {
    width = w;
}
double Object::getWidth( void ) {
    return width;
}
void Object::setHeight( double h ) {
    height = h;
}
double Object::getHeight( void ) {
    return height;
}
void Object::setCenter( double x, double y ) {
    center.x = x;
    center.y = y;
    center.z = 0;
}
double Object::getCenterX( void ) {
    return center.x;
}
double Object::getCenterY( void ) {
    return center.y;
}
double Object::getVelocityX( void ) {
    return velocity.x;
}
double Object::getVelocityY( void ) {
    return velocity.y;
}
double Object::getCameraX( void ) {
    return camera.x;
}
double Object::getCameraY( void ) {
    return camera.y;
}
void Object::setVelocityX( double x ) {
    velocity.x = x;
}
void Object::setVelocityY( double y ) {
    velocity.y = y;
}
void Object::setCameraX( double x ) {
    camera.x = x;
}
void Object::setCameraY( double y ) {
    camera.y = y;
}


double Object::getOldCenterX(void){
  return oldCenter.x;
}
double Object::getOldCenterY(void){
  return oldCenter.y;
}
void Object::setOldCenter(void){
  oldCenter.x = center.x;
  oldCenter.y = center.y;
  oldCenter.z = 0;
}

double Object::getTop(){
  return center.y + height;
}
double Object::getBottom(){
  return center.y - height;
}
double Object::getRight(){
  return center.x + width;
}
double Object::getLeft(){
  return center.x - width;
}

double Object::getOldTop(){
  return oldCenter.y + height;
}
double Object::getOldBottom(){
  return oldCenter.y - height;
}
double Object::getOldRight(){
  return oldCenter.x + width;
}
double Object::getOldLeft(){
  return oldCenter.x - width;
}

<<<<<<< HEAD
void Object::setIndex(int ind){
  index = ind;
=======

// screen boundaries for scrolling
double Object::getWindowCenterX(){
  return windowCenter.x;
}
double Object::getWindowCenterY(){
  return windowCenter.y;
}
void Object::scrollHorizontal(double amount){
  windowCenter.x += amount;
}
void Object::scrollVertical(double amount){
  windowCenter.y += amount;
>>>>>>> 012bf37183c4fe9ce47174f07895c77f8c2e661a
}
int Object::getIndex() {
    return index;
}
