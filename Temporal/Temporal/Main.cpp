#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "Math.h"
#include "Vector.h"
//#include "AnimationEditor.h"
#include "GameStateEditor.h"

using namespace Temporal;

int main(int argc, char* argv[])
{
	//GameStateManager::get().setListener(new AEGameStateListener());
	GameStateManager::get().setListener(new GSEGameStateListener());
	//Game::get().run("resources/game-states/animation-editor.xml");
	Game::get().run("resources/game-states/entities.xml");
	
//	_CrtDumpMemoryLeaks();
	return 0;
}

