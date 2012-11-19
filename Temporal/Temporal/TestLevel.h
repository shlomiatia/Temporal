#ifndef TESTLEVEL_H
#define TESTLEVEL_H

#include "Game.h"

namespace Temporal
{
	// General
	// TODO: Deal with component order

	// Physics
	// TODO: Store YABP in grid. Raise events
	// TODO: Fall from sloped cliff */[ ]
	// TODO: Walk/jump towards steep slope _*/ _*\
	// TODO: Jump while slide
	

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
	// TODO: Continues temporal echo memory
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
	};
}

#endif