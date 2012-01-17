#pragma once

#include "EntityStateID.h"
#include "EntityState.h"

namespace Temporal
{
	class EntityStatesFactory
	{
	public:
		static EntityStatesFactory& get(void)
		{
			static EntityStatesFactory instance;
			return (instance);
		}

		EntityState* getInitialState(void) const { return getState(EntityStateID::STAND); }
		EntityState* getState(EntityStateID::Type stateType) const;
		void releaseState(EntityState* state) const {};
	private:
		static const int MAX_ELEMENTS = 20;

		// TODO: Dynamic
		EntityState* _elements[MAX_ELEMENTS];
		int _elementsCount;

		EntityStatesFactory(void);
		~EntityStatesFactory(void);
		EntityStatesFactory(const EntityStatesFactory&) {};
		EntityStatesFactory& operator=(const EntityStatesFactory&) {};
	};
}

