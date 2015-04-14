// Class Ground
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600

class Ground {

    public:
        void setWidth(double w);
        double getWidth(void);
        void setHeight(double w);
        double getHeight(void);
        void setCenter(double x, double y);
        //Vec getCenter(void);
        //Ground( double w, double h, double x, double y);
        //~Ground(){ delete [] ground;}

    private:
        int width;
        int height;
        //std::vector<double>* center;

};

Ground::Ground( double w, double h, double x, double y ) {
    //Declare a ground segment
    //width  = 600;
    //height = 10;
    //center.x = WINDOW_WIDTH/2;
    //center.y = 0;
    //center.z = 0;

    //Declare a ground segment
    width  = w;
    height = h;
    //center.x = x;
    //center.y = y;
    //center.z = 0;
}

void Ground::setWidth( double w ) {
    width = w;
}
double Ground::getWidth( void ) {
    return width;
}

void Ground::setHeight( double h ) {
    height = h;
}
double Ground::getHeight( void ) {
    return height;
}

void Ground::setCenter( double x, double y ) {
    //center.x = x;
    //center.y = y;
    //center.z = 0;
}
//std::vector<double>* Ground::getCenter( void ) {
    //return center;
//}
