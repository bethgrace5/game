#ifndef PLAYER_H
#define PLAYER_H
//=====================================================================
//Player
//=====================================================================
class Player: public Object, public Sprite{
  private:
    int lives, health;
    int jumps, jumpLimit, jumpPower;
    int maxSpeed, speed;

    int index;
    bool cycleStop;

    timeval seqStart, seqEnd;

  public:
    Player();
    Player(int width, int height, int x, int y);

    void moveLeft();
    void moveRight();
    void jump();
    void stop();
    void jumpRefresh();

    void drawBox();
    void cycleAnimations();

    int diff_ms (timeval t1, timeval t2);
};
//==============================================
//Setup
//==============================================
Player::Player() : Object(26, 44, 250, 250), Sprite(){
  //Set The Default stats
  lives = 3; health = 3;

  jumps = 1; jumpLimit = 3; jumpPower = 7;
  speed = 0; maxSpeed  = 7;

  index = 0; cycleStop = 0;
}
//===============================================
//Movement Functions
//===============================================
void Player::moveRight(){ 
  Object::velocity.x = maxSpeed;
}
void Player::moveLeft(){
  Object::velocity.x = (-maxSpeed);
}
void Player::jump(){
  if(jumps < jumpLimit) Object::velocity.y = jumpPower;
}
void Player::stop(){
  Object::velocity.x = 0;
}
void Player::jumpRefresh(){ 
  if(Player::velocity.x == 0) jumps = 0; 
}
//===============================================
//Drawing Functions
//===============================================
void Player::cycleAnimations(){
  //Right Now There Are 13
  if(cycleStop == 1) return;
  if(health == 0){
 
  }
  else if(Object::getVelocityY() > 0){
    index = 8; 
  }
  else if(Object::getVelocityX() != 0){
    if(index < 7) index++;
    else index = 0;
  }
  else index = 0; 

  //if(index < 13) index++;
  //else index = 0;
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

  Sprite::drawTile(index);

  glPopMatrix();
}
//==============================================
//Helper Functions
//==============================================
int Player::diff_ms (timeval t1, timeval t2) {
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}
#endif
