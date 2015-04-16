// Class Ground
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>

class Ground {
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
        void setCenter(double x, double y);
        Ground( double w, double h, double x, double y);
        //~Ground(){ delete [] ground;}

    private:
        int width;
        int height;
        Vec center;

};

Ground::Ground( double w, double h, double x, double y ) {
    width  = w;
    height = h;
    center.x = x;
    center.y = y;
    center.z = 0;
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
    center.x = x;
    center.y = y;
    center.z = 0;
}
double Ground::getCenterX( void ) {
    return center.x;
}
double Ground::getCenterY( void ) {
    return center.y;
}
