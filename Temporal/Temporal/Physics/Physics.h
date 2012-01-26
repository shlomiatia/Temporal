#pragma once

#include "Body.h"

namespace Temporal
{
	class Physics
	{
	public:
		static const float GRAVITY;
		static const float MAX_GRAVITY;
		static const int MAX_ELEMENTS = 10;

		static Physics& get(void)
		{
			static Physics instance;
			return (instance);
		}

		Physics(void)
			: _elementsCount(0) {};
		~Physics(void) { for(int i = 0; i < _elementsCount; ++i) { delete _elements[i]; } }

		void add(Body* const element) { _elements[_elementsCount++] = element; }

		bool rayCast(const Body& source, const Body& destination) const;
		void update(void);

		Body* _elements[MAX_ELEMENTS];
		int _elementsCount;
	private:

		void processBodies(bool isDynamic, void (Physics::*processBody)(Body&, void*), void* param = NULL);
		void correctCollision(Body& staticBody, void* param);
		void detectCollision(Body& staticBody, void* param);
		void processCollisions(Body& dynamicBody, void* param);
		void processSensor(Body& staticBody, void* param);
		void processSensors(Body& dynamicBody, void* param);

		Physics(const Physics&);
		Physics& operator=(const Physics&);
	};
}
