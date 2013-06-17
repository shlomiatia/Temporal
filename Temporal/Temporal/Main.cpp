#include "Main.h"
#include "Game.h"
#include <SDL.h>

using namespace Temporal;

int main(int argc, char* argv[])
{
	Game::get().run("resources/game-states/loading.xml");
//	_CrtDumpMemoryLeaks();
	return 0;
}

