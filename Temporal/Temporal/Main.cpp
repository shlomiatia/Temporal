#include "Main.h"
#include "TestPanel.h"
#include <Temporal\Game\Game.h>
#include <SDL.h>

using namespace Temporal;

int main(int argc, char* argv[])
{	
	Panel* panel = new TestPanel();
	Game::get().setPanel(panel);
	Game::get().run();
	delete panel;
	return 0;
}
