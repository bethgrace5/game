//Storage
#include "definitions.h"
//#define BACKUP
//====================================================================
//Platform Storage
//====================================================================
struct data{
  Platform grounds[MAX_GROUNDS];
  int grounds_length;
  int heroAtX, heroAtY;
} storeIn, storeBack;

#ifdef BACKUP
struct data2{
  Platform grounds[500];
  int grounds_length;
  int heroAtX, heroAtY;
} futureBox;
#endif
