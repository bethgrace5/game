#include <stdio.h>

#include "GameEngine.h"
#include "GameState.h"

#define WINDOW_WIDTH  900
#define WINDOW_HEIGHT 600
using namespace std;


void GameEngine::Init() {
// initXWindows
//void initXWindows(void) { //do not change


}

void GameEngine::Cleanup() {
	// cleanup the all states
	while ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

    //clean up x windows
//void cleanupXWindows(void) { //do not change
//}
}

void GameEngine::ChangeState(GameState* state) {
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init();
}

void GameEngine::PushState(GameState* state) {
	// pause current state
	if ( !states.empty() ) {
		states.back()->Pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->Init();
}

void GameEngine::PopState() {
	// cleanup the current state
	if ( !states.empty() ) {
		states.back()->Cleanup();
		states.pop_back();
	}

	// resume previous state
	if ( !states.empty() ) {
		states.back()->Resume();
	}
}

void GameEngine::HandleEvents() {
	// let the state handle events
	states.back()->HandleEvents(this);
}

void GameEngine::Update() {
	// let the state update the game
	states.back()->Update(this);
}

void GameEngine::Draw() {
	// let the state draw the screen
	states.back()->Draw(this);
}
