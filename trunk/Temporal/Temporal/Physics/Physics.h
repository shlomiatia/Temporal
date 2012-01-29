#pragma once

#include "Body.h"
#include "DynamicBody.h"

namespace Temporal
{
	class Physics
	{
	public:
		static const float GRAVITY;
		static const float MAX_GRAVITY;
		static const int MAX_ELEMENTS = 20;

		static Physics& get(void)
		{
			static Physics instance;
			return (instance);
		}

		Physics(void)
			: _staticBodiesCount(0), _dynamicBodiesCount(0) {};
		~Physics(void) { for(int i = 0; i < _staticBodiesCount; ++i) { delete _staticBodies[i]; } for(int i = 0; i < _dynamicBodiesCount; ++i) { delete _dynamicBodies[i]; }  }

		void add(StaticBody* const element) { _staticBodies[_staticBodiesCount++] = element; }
		void add(DynamicBody* const element) { _dynamicBodies[_dynamicBodiesCount++] = element; }

		bool rayCast(const Body& source, const Body& destination) const;
		void update(void);

		StaticBody* _staticBodies[MAX_ELEMENTS];
		int _staticBodiesCount;
		DynamicBody* _dynamicBodies[MAX_ELEMENTS];
		int _dynamicBodiesCount;
	private:
		void processCollisions(DynamicBody& dynamicBody);
		void processSensors(DynamicBody& dynamicBody);

		Physics(const Physics&);
		Physics& operator=(const Physics&);
	};
}
