#ifndef TESTLEVEL_H
#define TESTLEVEL_H

#include "Game.h"

namespace Temporal
{
	
	class TestLevel : public Level
	{
	public:
		void init();
		void update(float framePeriod);
		virtual void draw() const;
		void dispose();
	};
}

#endif