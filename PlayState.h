#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include "GameState.h"
#include "ppm.h"
#include "Object.h"
extern "C" {
#include "fonts.h"
}

class PlayState : public GameState
{
public:
	void Init();
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(GameEngine* game);
	void Update(GameEngine* game);
	void Draw(GameEngine* game);
    Object createAI( int w, int h, Object *ground);
    void moveWindow(Object *hero);
    void groundCollide(Object *hero, Object *ground);
    bool detectCollide(Object *hero, Object *ground);
    void movement(Object *hero);
    unsigned char *buildAlphaData(Ppmimage *img);
    int diff_ms(timeval t1, timeval t2);
    int check_mouse(XEvent *e);
    int check_keys(XEvent *e, Object *hero, GameEngine* game);
    void drawGround(int x, int y);
    void drawEnemy(int x, int y);
    void drawHero(int x, int y);
    void drawBackground();
    void drawFonts();

	static PlayState* Instance() {
	    return &m_PlayState;
	}


protected:
	PlayState() { }

private:
	static PlayState m_PlayState;

};

#endif
