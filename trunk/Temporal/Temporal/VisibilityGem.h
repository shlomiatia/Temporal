#ifndef VISIBILITY_GEM_H
#define VISIBILITY_GEM_H

#include "EntitySystem.h"

namespace Temporal
{
	class VisibilityGem : public Component
	{
	public:
		VisibilityGem() : _isLit(true), _isVisible(true) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new VisibilityGem(); }
		static const Hash TYPE;
	private:

		bool _isVisible;
		bool _isLit;
	};
}
#endif