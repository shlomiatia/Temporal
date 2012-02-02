#pragma once

#include <vector>
#include <Temporal\Game\Game.h>
#include <Temporal\Game\Entity.h>

namespace Temporal
{
	class TestPanel : public Panel
	{
	public:
		TestPanel(void) {}

		void init(void);
		void update(void);
		void draw(void);
		void dispose(void);
	};
}

