#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "Temp3.h"

using namespace Temporal;

int main(int argc, char* argv[])
{
	GameStateManager::get().setListener(new MyGameStateListener());
	Game::get().run("resources/game-states/entities.xml");
//	_CrtDumpMemoryLeaks();
	return 0;
}

