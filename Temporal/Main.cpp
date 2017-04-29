#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "AnimationEditor.h"
#include "GameStateEditor.h"

using namespace Temporal;

int main(int argc, char* argv[])
{
	
	//GameStateManager::get().setListener(new BasicGameStateListener());
	//Game::get().run("resources/game-states/entities.xml");

	GameStateManager::get().setListener(new GSEGameStateListener());
	Game::get().run("resources/game-states/save-test.xml");

	//GameStateManager::get().setListener(new AEGameStateListener());
	//Game::get().run("resources/game-states/animation-editor.xml");
	
	//_CrtDumpMemoryLeaks();
	return 0;
}

