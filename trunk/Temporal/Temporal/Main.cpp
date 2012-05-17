#include "Main.h"
#include "TestPanel.h"
#include "Game.h"
#include <SDL.h>

using namespace Temporal;

int main(int argc, char* argv[])
{	
	Panel* panel = new TestPanel();
	Game::get().setPanel(panel);
	Game::get().run();
	delete panel;
	_CrtDumpMemoryLeaks();
	return 0;
}

