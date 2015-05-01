#ifndef INTROSTATE_H
#define INTROSTATE_H

#include "GameState.h"

class IntroState : public GameState
{
public:
	void Init();
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(GameEngine* game);
	void Update(GameEngine* game);
	void Draw(GameEngine* game);
    int check_mouse(XEvent *e);
    int check_keys(XEvent *e, GameEngine *game);

	static IntroState* Instance() {
		return &m_IntroState;
	}

protected:
	IntroState() { }

private:
	static IntroState m_IntroState;

};

#endif
