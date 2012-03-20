#ifndef TESTPANEL_H
#define TESTPANEL_H

#include <Temporal\Game\Panel.h>

// TODO: Architecture SLOTH
// TODO: Separate common panel SLOTH
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

#endif