#include "EntityStatesFactory.h"
#include "EntityStates.h"
#include <assert.h>

namespace Temporal
{
	EntityStatesFactory::EntityStatesFactory(void)
			: _elementsCount(0) 
	{
		_elements[_elementsCount++] = new Stand();
		_elements[_elementsCount++] = new Fall();
		_elements[_elementsCount++] = new Walk();
		_elements[_elementsCount++] = new Turn();
		_elements[_elementsCount++] = new JumpStart();
		_elements[_elementsCount++] = new JumpUp();
		_elements[_elementsCount++] = new JumpForward();
		_elements[_elementsCount++] = new JumpForwardEnd();
		_elements[_elementsCount++] = new Hanging();
		_elements[_elementsCount++] = new Hang();
		_elements[_elementsCount++] = new Drop();
		_elements[_elementsCount++] = new Climbe();
		_elements[_elementsCount++] = new PrepareToDescend();
		_elements[_elementsCount++] = new Descend();
		assert(_elementsCount <= MAX_ELEMENTS);
		
	}

	EntityStatesFactory::~EntityStatesFactory(void)
	{
		for(int i = 0; i < _elementsCount; ++i)
		{
			delete _elements[i]; 
		}
	}

	EntityState* EntityStatesFactory::getState(EntityStateID::Type stateType)
	{
		return _elements[stateType];
	}
}
