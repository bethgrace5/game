#ifndef OBJECT_H
#define OBJECT_H

// Class Object
// can be used to create AI sprites as well

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

        // used to update boundaries for window scrolling
        double getWindowCenterX();
        double getWindowCenterY();
        void scrollHorizontal(double amount);
        void scrollVertical(double amount);
        double getWindowInterval();

        Object( double w, double h, double x, double y);
        // TODO: create destructor
        //~Object(){ delete [] ground;}

    private:
        int width;
        int height;
        // interval is distance the sprite is allowed to move away
        // from windowCenter
        int interval;
        Vec camera;
        Vec velocity;
        Vec center;
        Vec oldCenter;
        Vec windowCenter;
};

#endif
