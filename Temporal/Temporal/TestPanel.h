#ifndef TESTPANEL_H
#define TESTPANEL_H

#include <Temporal\Game\BasePanel.h>

namespace Temporal
{
	// ASSUMPTION: When trying to walk towards a steep hill, the entity will raise a bit */																															 																								
	// ASSUMPTION: When trying to walk on a flat floor where the ceiling is sloped so there's no room, the entity will go down a bit *\
	// ASSUMPTION: When trying to walk on a sloped floor where the ceiling is flat so there's no room, the entity will shake */

	// TODO: Handle areas intersection in navigation graph PHYISICS
	// TODO: Support directed jump at jump forward for platforms as well as ledges. Jump without hang PHYSICS
	// TODO: Buffer/query current sensors (drop) PHYSICS
	
	// TODO: Use hashed string for Entity, components, sensors, messages, states etc. ENTITIES
	// TODO: Pass static entity id in sensor, instead of the static body itself ENTITIES
	// TODO: Don't hard code IDs (Sight, View Manager) ENTITIES
	// TODO: Deal with component order ENTITIES

	// TODO: Consider transition type in navigation edge cost  AI
	// TODO: Fix huristics:  Take into account position, need for turning, and several edges to the same platform AI
	// TODO: Fault tolerance in navigator AI
	// TODO: Use entitiy size for stuff. Allow to hang on forward jumps AI

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