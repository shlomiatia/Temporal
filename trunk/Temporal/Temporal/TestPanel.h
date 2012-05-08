#ifndef TESTPANEL_H
#define TESTPANEL_H

#include <Temporal\Game\BasePanel.h>

namespace Temporal
{
	// ASSUMPTION: When trying to walk towards a steep hill, the entity will raise a bit */																															 																								
	// ASSUMPTION: When trying to walk on a flat floor where the ceiling is sloped so there's no room, the entity will go down a bit *\
	// ASSUMPTION: When trying to walk on a sloped floor where the ceiling is flat so there's no room, the entity will shake */

	// TODO: Check sensor each movement step PHYSICS
	// TODO: Insert segments to grid using grid traversale algorithm PHYSICS
	// TODO: Store YABP in grid PHYSICS
	// TODO: Buffer/query current sensors (drop) PHYSICS
	
	// TODO: Use hashed string for components, messages, states, animation id etc. ENTITIES
	// TODO: Pass static entity id in sensor, instead of the static body itself ENTITIES
	// TODO: Deal with component order ENTITIES

	// TODO: Consider transition type in navigation edge cost  AI
	// TODO: Fix huristics:  Take into account position, need for turning, and several edges to the same platform AI
	// TODO: Fault tolerance in navigator AI

	// TODO: Create arbitrary points on entity (draw position, eyes) GRAPHICS
	// TODO: Gradual hang & descend GRAPHICS
	// TODO: Test against top, center bottom in sight GRAPHICS

	// TODO: Buffer/query current actions (descend, walk), or create another descending proxy state INPUT

	// TODO: Separate action controller to pluggable modules SCRIPTS

	// TODO: Enable/Disable Sensors PERFORMACE
	// TODO: Request sight PERFORMANCE
	// TODO: Cache ray properties PERFORMANCE
	class TestPanel : public BasePanel
	{
	public:
		void createEntities(void);
	};
}

#endif