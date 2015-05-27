//Storage
#define MAX_GROUNDS 100
struct data{
  Platform grounds[MAX_GROUNDS];
  int grounds_length;
  int heroAtX, heroAtY;
} storeIn, storeBack;

struct data2{
  Sprite spriteSheet[10]; 
  Platform2 grounds[MAX_GROUNDS];
  int grounds_length; int spriteSheet_length;

  data2(){
    grounds_length = spriteSheet_length = 0; 
  }

  void reInit(){
    //Reinput The Sprites Again
    for(int i = 0; i < 10; i++){
      spriteSheet[i].reInitSprite(); 
    }
    //Reput The spriteSheet to the each platforms.
    for(int i = 0; i < MAX_GROUNDS; i++){
      grounds[i].setSprite
        (spriteSheet[grounds[i].getSpriteID()]);
    }
  }
} boxA, boxB;
