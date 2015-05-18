#include "Object.h"

Object::Object( double w, double h, double x, double y ) {
  init(w,h,x,y);
}

void Object::init( double w, double h, double x, double y ) {
    width  = w;
    height = h;
    center.x = x;
    center.y = y;
    center.z = 0;

    velocity.x = 0;
    velocity.y = 0;
    velocity.z = 0;

    life=100;

    index = 0;
    left=w*-1;
    bottom=h*-1;
    right=w;
    top=h;
    floor=NULL;
    jump=0;
    aggro=false;
    isJumping=0;
    isFalling=0;
    isWalking=0;
    isShooting=0;
    isDying=0;
    isDead=0;
}

void Object::autoSet(){
  center.x += velocity.x;
  center.y += velocity.y;
  center.z += velocity.z;
}


void Object::setAggro( bool b ) {
    aggro = b;
}
bool Object::getAggro( void ) {
    return aggro;
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
void Object::setJump() {
    jump++;
}
int Object::getJump() {
    return jump;
}
double Object::getVelocityX( void ) {
    return velocity.x;
}
double Object::getVelocityY( void ) {
    return velocity.y;
}
void Object::setVelocityX( double x ) {
    velocity.x = x;
}
void Object::setVelocityY( double y ) {
    velocity.y = y;
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

void Object::setTop( double t ) {
    top = t;
}
void Object::setRight( double r ) {
    right = r;
}
void Object::setLeft( double l ) {
    left = l;
}
void Object::setBottom( double b ) {
    bottom = b;
}
double Object::getTop(){
  return center.y + top;
}
double Object::getBottom(){
  return center.y + bottom;
}
double Object::getRight(){
  return center.x + right;
}
double Object::getLeft(){
  return center.x + left;
}

double Object::getOldTop(){
  return oldCenter.y + top;
}
double Object::getOldBottom(){
  return oldCenter.y + bottom;
}
double Object::getOldRight(){
  return oldCenter.x + right;
}
double Object::getOldLeft(){
  return oldCenter.x + left;
}

void Object::setIndex(int ind){
  index = ind;
}
int Object::getIndex() {
    return index;
}
void Object::setFloor(Object *obj){
  floor = obj;
  if (obj)
    jump=0;
}
Object *Object::getFloor() {
    return floor;
}
