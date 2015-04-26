#include "Object.h"

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
    index = 0;
    windowCenter.x = 300;
    windowCenter.y = 300;
    windowCenter.z = 0;
    interval = 50;
    left=w*-1;
    bottom=h*-1;
    right=w;
    top=h;
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
}
int Object::getIndex() {
    return index;
}
