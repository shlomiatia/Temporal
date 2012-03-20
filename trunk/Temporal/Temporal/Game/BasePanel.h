#ifndef BASEPANEL_H
#define BASEPANEL_H

#include "Panel.h"

namespace Temporal
{
	class BasePanel : public Panel
	{
	public:
		void init(void);
		void update(float framePeriodInMillis);
		void draw(void) const;
		void dispose(void);

	protected:
		virtual void createEntities(void) = 0;
	};
}

#endif