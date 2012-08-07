#ifndef TESTPANEL_H
#define TESTPANEL_H

#include "BasePanel.h"

namespace Temporal
{
	// ASSUME: When trying to walk towards a steep hill, the entity will raise a bit */																															 																								
	// ASSUME: When trying to walk on a flat floor where the ceiling is sloped so there's no room, the entity will go down a bit *\
	// ASSUME: When trying to walk on a sloped floor where the ceiling is flat so there's no room, the entity will shake */

	// TODO: Graphics utils. line renderer scene graph	
	// TODO: Normalize positions - attch to scene graph
	// TODO: Minimize casting

	// TODO: Check sensor each movement step PHYSICS
	// TODO: Insert segments to grid using grid traversale algorithm PHYSICS
	// TODO: Store YABP in grid PHYSICS
	
	// TODO: Deal with component order ENTITIES

	// TODO: Consider transition type in navigation edge cost  AI
	// TODO: Fix huristics:  Take into account position, need for turning, and several edges to the same platform AI
	// TODO: Fault tolerance in navigator AI

	// TODO: Create arbitrary points on entity (draw position, eyes) GRAPHICS
	// TODO: Gradual hang & descend GRAPHICS
	// TODO: Test against top, center bottom in sight GRAPHICS
	// TODO: Gradual temporal echo merge GRAPHICS
	// TODO: Soft shadows GRAPHICS
	 
	// TODO: Enable/Disable Sensors PERFORMACE
	// TODO: Request sight PERFORMANCE
	// TODO: Cache ray properties PERFORMANCE
	// TODO: Do not process collision twice PERFORMANCE
	// TODO: Less temporal echo memory PERFORMANCE
	// TODO: Delete spritesheets, singletons, static stl containers PERFORMANCE
	// TODO: Cache shadows/display list/FBO/circle grid/light culling/save texture PERFORMANCE
	class TestPanel : public BasePanel
	{
	public:
		void createEntities();
	};
}

#endif