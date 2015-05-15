#ifndef PLAYER_H
#define PLAYER_H
//=====================================================================
//Player
//=====================================================================
class Player: public Object{
  private:
    std::string name;

    int lives, health;

    int jumps, jumpLimit, jumpPower;

    int maxSpeed, speed;

  public:
    Player();
    Player(int width, int height, int x, int y);

    void moveLeft();
    void moveRight();
    void jump();
    void stop();
    void jumpRefresh();
    void autoSet();

    void drawBox();
};
//==============================================
//Setup
//==============================================
Player::Player() : Object(26, 44, 250, 250){
  name = "No Name";
  //Basic Status
  lives = 3; health = 3;

  jumps = 1; jumpLimit = 3; jumpPower = 5;
  speed = 0; maxSpeed  = 7;
}
//===============================================
//Movement Functions
//===============================================
void Player::moveRight(){ 
  Object::setVelocityX(maxSpeed);
}
void Player::moveLeft(){
  Object::setVelocityX(-maxSpeed);
}
void Player::jump(){
  Object::setVelocityY(jumpPower);
}
void Player::stop(){
  Object::setVelocityX(0);
}
void Player::jumpRefresh(){ 
  if(Player::getVelocityY() == 0) jumps = 0; 
}

void Player::autoSet(){
  Object::setCenter(Object::getCenterX() + Object::getVelocityX(), 
                    Object::getCenterY() + Object::getVelocityY() );

}
//===============================================
//Drawing Functions
//===============================================
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
