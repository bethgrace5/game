#ifndef PLAYER_H
#define PLAYER_H
//=====================================================================
//Player
//=====================================================================
class Player: public Object{
  private:
    std::string name;

    int life, health;

    int jumps, jumpLimit, jumpPower;

    int maxSpeed, speed;

  public:
    Player();
    void move(int x);
    void jumpRefresh();
    void drawMovement();
    void drawBox();
};

Player::Player() : Object(46, 48, 250, 250){
  name = "No Name";
  //Basic Status
  life = 3; health = 3;

  jumps = 1; jumpLimit = 2; jumpPower = 5;
  speed = 0; maxSpeed  = 5;

  Object::setTop(44);
  Object::setBottom(44);
  Object::setLeft(-26);
  Object::setRight(26);
}

void Player::jumpRefresh(){ 
  if(Player::getVelocityY() == 0) jumps = 0; 
}

void Player::move(int x){
  switch(x){
    case  1: Object::setVelocityX(maxSpeed);    break;
    case -1: Object::setVelocityX(-(maxSpeed)); break;
    case  0: Object::setVelocityX(0);           break;

    case  2: 
             if(jumps >= jumpLimit) break;
             Object::setVelocityY(jumpPower);
             break;

    //case -2: Object::setVelocityY(-(jumpPower)); break;
  }
}

void Player::drawBox(){
  int w = Object::getWidth();
  int h = Object::getHeight();
  glPushMatrix();
  glTranslatef(Object::getCenterX(), Object::getCenterY(), 0);

  glBegin(GL_QUADS);
  glVertex2i(-w, -h);
  glVertex2i(-w,  h);
  glVertex2i( w,  h);
  glVertex2i( w, -h);
  glEnd(); 
  //Sprite::drawTile(0);

  glPopMatrix();
}

#endif
