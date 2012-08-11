#ifndef TESTPANEL_H
#define TESTPANEL_H

#include "BasePanel.h"

namespace Temporal
{
	// ASSUME: When trying to walk towards a steep hill, the entity will raise a bit */																															 																								
	// ASSUME: When trying to walk on a flat floor where the ceiling is sloped so there's no room, the entity will go down a bit *\
	// ASSUME: When trying to walk on a sloped floor where the ceiling is flat so there's no room, the entity will shake */

	// TODO: Minimize casting
	// TODO: message utils
	// TODO: sprite size

	// General
	// TODO: Text
	// TODO: Files
	// TODO: Resources
	// TODO: Input
	// TODO: systems
	// TODO: games states/panels
	// TODO: animations
	// TODO: Deal with component order

	// Physics
	// TODO: moving platforms
	// TODO: Sensor/Button
	// TODO: Check sensor each movement step
	// TODO: Insert segments to grid using grid traversale algorithm
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
	// TODO: Less temporal echo memory
	// TODO: Culling
	// TODO: Delete spritesheets, singletons, static stl containers
	// TODO: Cache shadows/display list/FBO/circle grid/light culling/save texture
	class TestPanel : public BasePanel
	{
	public:
		void draw() const;
	protected:
		void createEntities();
	};
}

#endif