#ifndef VISIBILITY_GEM_H
#define VISIBILITY_GEM_H

#include "EntitySystem.h"
#include "Ids.h"

namespace Temporal
{
	class VisibilityGem : public Component
	{
	public:
		VisibilityGem() : _isLit(true), _isCovered(true), _isVisible(true) {}

		Hash getType() const { return ComponentsIds::VISIBILITY_GEM; }
		void handleMessage(Message& message);

		Component* clone() const { return new VisibilityGem(); }
		
	private:
		bool _isVisible;
		bool _isLit;
		bool _isCovered;
	};
}
#endif