#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Game\Game.h>
#include <Temporal\Physics\Body.h>
#include <Temporal\Game\BackgroundEntity.h>
#include <Temporal\Game\DynamicEntity.h>

namespace Temporal
{
	class TestPanel : public Panel
	{
	public:
		TestPanel(void) : _elementsCount(0) {}

		void init(void);
		void update(void);
		void draw(void);
		void dispose(void);

		static const int MAX_ELEMENTS = 10;
		VisualEntity* _elements[MAX_ELEMENTS];
		int _elementsCount;
	};
}

