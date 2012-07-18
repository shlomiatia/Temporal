#ifndef BASEPANEL_H
#define BASEPANEL_H

#include "Panel.h"

namespace Temporal
{
	class BasePanel : public Panel
	{
	public:
		void init();
		void update(float framePeriodInMillis);
		void draw() const;
		void dispose();

	protected:
		virtual void createEntities() = 0;
	};
}

#endif