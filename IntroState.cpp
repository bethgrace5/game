#include <stdio.h>

#include "GameEngine.h"
#include "GameState.h"
#include "IntroState.h"
#include "PlayState.h"

IntroState IntroState::m_IntroState;

void IntroState::Init()
{

}

void IntroState::Cleanup()
{
}

void IntroState::Pause()
{
}

void IntroState::Resume()
{
}

void IntroState::HandleEvents(GameEngine* game)
{
    game->ChangeState( PlayState::Instance() );
}

void IntroState::Update(GameEngine* game) 
{
}

void IntroState::Draw(GameEngine* game) 
{
}
