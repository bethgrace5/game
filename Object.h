#ifndef OBJECT_H
#define OBJECT_H

// Class Object
// can be used to create AI sprites as well

class Object{
    struct Vec {
        float x, y, z;
    };
    public:
        int life;
        void setWidth(double w);
        double getWidth(void);
        void setHeight(double w);
        double getHeight(void);

        double getCenterX(void);
        double getCenterY(void);

        double getVelocityX(void);
        double getVelocityY(void);

        void setJump(void);
        int getJump(void);

        void setAggro(bool b);
        bool getAggro(void);

        void setCenter(double x, double y);

        void setVelocityX(double x);
        void setVelocityY(double y);
        void autoSet();

        double getOldCenterX(void);
        double getOldCenterY(void);
        void  setOldCenter(void);

        void setTop(double t);
        void setRight(double r);
        void setLeft(double l);
        void setBottom(double b);
        double getTop(void);
        double getRight(void);
        double getBottom(void);
        double getLeft(void);

        double getOldTop(void);
        double getOldRight(void);
        double getOldBottom(void);
        double getOldLeft(void);

        //Moved to Sprite Class
        void setIndex(int ind);
        int getIndex();

        void setID(int objID);
        int getID();

        void setFloor(Object *obj);
        Object *getFloor();
        
        int isJumping, isFalling, isDying, isWalking, isShooting, isDead;

        Object();
        Object( double w, double h, double x, double y);
        void init(double w, double h, double x, double y);
        void init(double w, double h);
        // TODO: create destructor
        //~Object(){ delete [] ground;}
       
        //virtual Object *Clone () = 0;
 
    private:
        int width;
        int height;
        int top, bottom, left, right;
        // interval is distance the sprite is allowed to move away
        // from windowCenter
        
        int index;
        int obj_id;;
        int jump;
        bool aggro;
        bool hostile;
        bool interactive;
        Vec velocity;
        Vec center;
        Vec oldCenter;
        Object *floor;        
};

#endif
