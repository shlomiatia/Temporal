#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "Math.h"
#include "Vector.h"
#include "AnimationEditor.h"

using namespace Temporal;

int main(int argc, char* argv[])
{
	GameStateManager::get().setListener(new MyGameStateListener());
	Game::get().run("resources/game-states/test.xml");
	
//	_CrtDumpMemoryLeaks();
	return 0;
}

