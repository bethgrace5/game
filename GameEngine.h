#ifndef GAMEENGINE_H
#define GAMEENGINE_H


#include <vector>
using namespace std;

class GameState;

class GameEngine
{
public:

    //X Windows variables
    Display *dpy;
    Window win;
    GLXContext glc;
    //XEvent *e; 

	void Init(GameEngine *game);
	void Cleanup();

	void ChangeState(GameState* state);
	void PushState(GameState* state);
	void PopState();

	void HandleEvents();
	void Update();
	void Draw();

	bool Running() { return m_running; }
	void Quit() { 
        // check if user really would like to quit
        m_running = false;
    }


private:
	// the stack of states
	vector<GameState*> states;

	bool m_running;
	bool m_fullscreen;
};

#endif
