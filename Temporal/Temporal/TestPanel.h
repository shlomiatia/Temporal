#pragma once
#include <Temporal\Base\Base.h>
#include <Temporal\Game\Game.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\Graphics\SpriteSheet.h>

namespace Temporal
{
	class TestPanel : public Panel
	{
	public:
		void init(void);
		void update(float framePeriodInMillis);
		void draw(void) const;
		void dispose(void);
	};
}

