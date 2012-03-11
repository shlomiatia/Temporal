#pragma once

#include <Temporal\Game\Game.h>
#include <Temporal\Physics\Grid.h>
#include <Temporal\Graphics\SpriteSheet.h>

// TODO: Architecture
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

