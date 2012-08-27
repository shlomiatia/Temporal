#include "Main.h"
#include "TestLevel.h"
#include "Game.h"
#include <SDL.h>

using namespace Temporal;

int main(int argc, char* argv[])
{	
	Game::get().setLevel(new TestLevel());
	Game::get().run();
//	_CrtDumpMemoryLeaks();
	return 0;
}

