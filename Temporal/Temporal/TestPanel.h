#pragma once

#include <Temporal\Game\Panel.h>

// TODO: Architecture SLOTH
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

