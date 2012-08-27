#ifndef BASELEVEL_H
#define BASELEVEL_H

#include "Game.h"

namespace Temporal
{
	class BaseLevel : public Level
	{
	public:
		void init();
		void update(float framePeriodInMillis);
		virtual void draw() const;
		void dispose();

	protected:
		virtual void createEntities() = 0;
	};
}

#endif