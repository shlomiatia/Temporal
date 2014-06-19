#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "Math.h"
#include "Vector.h"
//#include "AnimationEditor.h"
//#include "GameStateEditor.h"
//#include "Temp1.h"

using namespace Temporal;

int main(int argc, char* argv[])
{
	//GameStateManager::get().setListener(new AEGameStateListener());
	//GameStateManager::get().setListener(new GSEGameStateListener());
	//GameStateManager::get().setListener(new GSLGameStateListener());
	
	//Game::get().run("resources/game-states/animation-editor.xml");
	Game::get().run("resources/game-states/entities.xml");
	//Game::get().run("resources/game-states/loading.xml");
	
//	_CrtDumpMemoryLeaks();
	return 0;
}

