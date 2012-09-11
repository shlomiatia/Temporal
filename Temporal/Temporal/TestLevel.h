#ifndef TESTLEVEL_H
#define TESTLEVEL_H

#include "Game.h"

namespace Temporal
{
	// ASSUME: When trying to walk towards a steep hill, the entity will raise a bit */																															 																								
	// ASSUME: When trying to walk on a flat floor where the ceiling is sloped so there's no room, the entity will go down a bit *\
	// ASSUME: When trying to walk on a sloped floor where the ceiling is flat so there's no room, the entity will shake */

	// General
	// TODO: Serialize temp
	// TODO: Text
	// TODO: Input
	// TODO: Deal with component order

	// Physics
	// TODO: moving platforms
	// TODO: Store YABP in grid

	// Graphics
	// TODO: Scene graph attachment (draw position, eyes)
	// TODO: Gradual hang & descend
	// TODO: Test against top, center bottom in sight
	// TODO: Gradual temporal echo merge
	// TODO: Soft shadows

	// AI
	// TODO: Consider transition type in navigation edge cost
	// TODO: Fix huristics:  Take into account position, need for turning, and several edges to the same platform
	// TODO: Fault tolerance in navigator
	 
	// Performance
	// TODO: Enable/Disable Sensors
	// TODO: Request sight
	// TODO: Cache ray properties/global shape
	// TODO: Do not process collision twice
	// TODO: Less temporal echo memory. Use array instead of list
	// TODO: Culling
	// TODO: Delete spritesheets, singletons, static stl containers
	// TODO: Cache shadows/display list/FBO/save texture
	class TestLevel : public Level
	{
	public:
		void init();
		void update(float framePeriod);
		virtual void draw() const;
		void dispose();
	private:
		void createEntities();
	};
}

#endif