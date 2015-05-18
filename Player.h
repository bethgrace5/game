

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
    int once;

    timeval seqStart, seqEnd;

  public:
    Player();
    Player(int width, int height, int x, int y);

    void moveLeft();
    void moveRight();
    void jump();
    void stop();
    void jumpRefresh();

    void setHealth(int take);
    int getHealth();

    void drawBox();
    void cycleAnimations();
    void setOnGround(bool take);

    int diff_ms (timeval t1, timeval t2);
};
//==============================================
//Setup
//==============================================
Player::Player() : Object(26, 44, 250, 250), Sprite(){
  //Set The Default stats
  lives = 3; health = 3;

  jumps = 1; jumpLimit = 2; jumpPower = 7;
  speed = 0; maxSpeed  = 7;

  index = 0; once = 0;
}
//===============================================
//Movement Functions
//===============================================
void Player::moveRight(){ 
  Sprite::setMirror(false);
  Object::setVelocityX(maxSpeed);
}
void Player::moveLeft(){
  Sprite::setMirror(true);
  Object::setVelocityX(-maxSpeed);
}
void Player::jump(){
  if(jumps < jumpLimit){ 
      Object::setVelocityY(jumpPower); 
      jumps++;
  }
}
void Player::stop(){
  Object::setVelocityX(0);
}
void Player::jumpRefresh(){ 
  if(Player::getVelocityY() == 0) jumps = 0; 
}
//==============================================
//Stats Functions
//==============================================
void Player::setHealth(int take){
  health = take;
}
int Player::getHealth(){
  return health;
}
//===============================================
//Drawing Functions
//===============================================/
//    Note: This is a Prototype -
//          -Right now its specific number depedant.
//          -
//
//       Right Now There Are 13 sprites in 1 sheet
void Player::cycleAnimations(){
  //int tmpColumn = Sprite::getColumn();
  //int tmpRow = Sprite::getRow();

  //Start Timer
  if(once == 0){ gettimeofday(&seqStart, NULL); once = 1 ;}
  gettimeofday(&seqEnd, NULL);//This Can Be Universal For All Stuff

  //Death
  if(getHealth() == 0){
    if(diff_ms(seqEnd, seqStart) > 180){
      if(index == 12) return;
      if(index < 7) index = 7;
      index++; once = 0;
    }
  }
  //Jumping
  else if(Object::getVelocityY() > 0){
      if(jumps==2) {
          index=0;
      }
      else {
        index = 1;
      }
  }
  //Falling
  else if(Object::getVelocityY() < 0){
    index = 0;
  }
  //Walking/Running
  else if(Object::getVelocityX() != 0){
    if(diff_ms(seqEnd, seqStart) > 80){
      index = (index + 1) % 6;
      once = 0;
    }
  }
  //Standing
  else index = 6; 

  Sprite::setIndexAt(index);
}

void Player::drawBox(){
  //int w = Object::getWidth();
  //int h = Object::getHeight();
  glPushMatrix();
  glTranslatef(Object::getCenterX(), Object::getCenterY(), 0);

  /*glBegin(GL_QUADS);
  glVertex2i(-w, -h);
  glVertex2i(-w,  h);
  glVertex2i( w,  h);
  glVertex2i( w, -h);
  glEnd();*/

  Sprite::drawTile(Sprite::getIndexAt());

  glPopMatrix();
}
//==============================================
//Helper Functions
//==============================================
int Player::diff_ms (timeval t1, timeval t2) {
  return (((t1.tv_sec - t2.tv_sec) * 1000000) + (t1.tv_usec - t2.tv_usec))/1000;
}
#endif
