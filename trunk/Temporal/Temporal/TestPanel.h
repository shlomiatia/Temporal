#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Game\Game.h>
#include <Temporal\Physics\Body.h>
#include <Temporal\Game\Entity.h>

namespace Temporal
{
	class TestPanel : public Panel
	{
	public:
		void init(void);
		void update(void);
		void draw(void);
		void dispose(void);

		Entity* _player;
		Entity* _enemy;
	};
}

