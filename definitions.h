#ifndef DEFINITIONS_H
#define DEFINITIONS_H

//Development Controls

// comment to ignore sound use
#define USE_SOUND

// 1 for quick load, 0 for slow load with menu images
#define QUICK_LOAD_TIME 0

// 1 to use tool editor, 0 to use pre-defined objects
#define USE_TOOLS 1

//If 1, you can write the name of the save file
#define OPTIONAL_STORAGE 1

#define GRAVITY 0.2
#define HERO_START_X 150
#define HERO_START_Y 350
#define LEVEL_WIDTH 13500
#define MAX_BACKGROUND_BITS 1000
#define MAX_ENEMIES 200
#define MAX_GROUNDS 500
#define MAX_ATTACKS 25
#define MAX_CURRENTS 50
#define MAX_HEIGHT 2400
#define WINDOW_HALF_HEIGHT WINDOW_HEIGHT/2
#define WINDOW_HALF_WIDTH  WINDOW_WIDTH/2
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 900

#define rnd()(float)rand() /(float)(RAND_MAX)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]

#endif


