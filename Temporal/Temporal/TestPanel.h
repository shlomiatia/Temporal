#pragma once
#include <Temporal\Base\Base.h>
#include <Temporal\Game\Game.h>
#include <Temporal\Physics\StaticBodiesIndex.h>
#include <Temporal\Graphics\SpriteSheet.h>

namespace Temporal
{
	class TestPanel : public Panel
	{
	public:
		TestPanel(void) {}

		void init(void);
		void update(float framePeriodInMillis);
		void draw(void);
		void dispose(void);
	};
}

