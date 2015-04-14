// Class Sprite
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

class Sprite {
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
        Sprite( double w, double h, double x, double y);
        //~Sprite(){ delete [] ground;}

    private:
        int width;
        int height;
        Vec camera;
        Vec velocity;
        Vec center;

};

Sprite::Sprite( double w, double h, double x, double y ) {
        //Declare a sprite shape
        //width  = 50;
        //height = 50;
        //center.x = WINDOW_WIDTH/2;
        //center.y = 50;
        //center.z = 0;
        //camera.x = 0; 
        //camera.y = 0;

    //Declare a ground segment
    width  = w;
    height = h;
    center.x = x;
    center.y = y;
    center.z = 0;
}

void Sprite::setWidth( double w ) {
    width = w;
}
double Sprite::getWidth( void ) {
    return width;
}

void Sprite::setHeight( double h ) {
    height = h;
}
double Sprite::getHeight( void ) {
    return height;
}

void Sprite::setCenter( double x, double y ) {
    center.x = x;
    center.y = y;
    center.z = 0;
}
double Sprite::getCenterX( void ) {
    return center.x;
}
double Sprite::getCenterY( void ) {
    return center.y;
}
double Sprite::getVelocityX( void ) {
    return velocity.x;
}
double Sprite::getVelocityY( void ) {
    return velocity.y;
}
double Sprite::getCameraX( void ) {
    return camera.x;
}
double Sprite::getCameraY( void ) {
    return camera.y;
}
void Sprite::setVelocityX( double x ) {
    velocity.x = x;
}
void Sprite::setVelocityY( double y ) {
    velocity.y = y;
}
void Sprite::setCameraX( double x ) {
    camera.x = x;
}
void Sprite::setCameraY( double y ) {
    camera.y = y;
}
