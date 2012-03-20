#include "Sight.h"
#include "Grid.h"
#include <Temporal\Game\Message.h>
#include <Temporal\Game\EntitiesManager.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void Sight::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			checkLineOfSight();
		}
		else if(message.getID() == MessageID::DEBUG_DRAW)
		{
			drawDebugInfo();
		}
	}

	void Sight::checkLineOfSight(void)
	{
		_pointOfIntersection = Vector::Zero;
		_isSeeing = false;
		const Vector& sourcePosition = *(Vector*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		// TODO: Who're you watching? ENTITIES
		const Vector& targetPosition = *(Vector*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));


		// Check orientation
		if((targetPosition.getX() - sourcePosition.getX()) * sourceOrientation < 0.0f)
			return;

		// Check field of view
		// TODO: Test against top and bottom PHYSICS
		// TODO: Eyes GRAPHICS
		float slope = (targetPosition.getY() - sourcePosition.getY()) / (targetPosition.getX() - sourcePosition.getX());
		float angle = atan(slope);

		if ((_upperAngle * sourceOrientation - angle) * sourceOrientation < 0.0f ||
			(_lowerAngle * sourceOrientation - angle) * sourceOrientation > 0.0f)
			return;

		_isSeeing = rayCast(sourcePosition, targetPosition);
		
		if(_isSeeing)
			sendMessageToOwner(Message(MessageID::LINE_OF_SIGHT));
	}

	bool Sight::rayCast(const Vector& source, const Vector& destination)
	{
		float x1 = source.getX();
		float y1 = source.getY();
		float x2 = destination.getX();
		float y2 = destination.getY();
		
		int i = Grid::get().getAxisIndex(x1);
		int j = Grid::get().getAxisIndex(y1);

		// Determine end grid cell coordinates (iend, jend)
		int iend = Grid::get().getAxisIndex(x2);
		int jend = Grid::get().getAxisIndex(y2);

		// Determine in which primary direction to step
		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));

		const float tileSize = Grid::get().getTileSize();

		// Determine tx and ty, the values of t at which the directed segment
		// (x1,y1)-(x2,y2) crosses the first horizontal and vertical cell
		// boundaries, respectively. Min(tx, ty) indicates how far one can
		// travel along the segment and still remain in the current cell
		float minx = tileSize * floor(x1/tileSize), maxx = minx + tileSize;
		float tx = ((x1 < x2) ? (x1 - minx) : (maxx - x1)) / abs(x2 - x1);
		float miny = tileSize * floor(y1/tileSize), maxy = miny + tileSize;
		float ty = ((y1 < y2) ? (y1 - miny) : (maxy - y1)) / abs(y2 - y1);

		// Determine deltax/deltay, how far (in units of t) one must step
		// along the directed line segment for the horizontal/vertical
		// movement (respectively) to equal the width/height of a cell
		float deltatx = tileSize / abs(x2 - x1);
		float deltaty = tileSize / abs(y2 - y1);

		bool isSuccessful = true;
		// Main loop. Visits cells until last cell reached
		while(true)
		{

			// TODO: Compare with specific line PHYSICS
			StaticBodyCollection* staticBodies = Grid::get().getTile(i, j);
			if(staticBodies != NULL)
			{
				isSuccessful = false;
				break;
			}
			if (tx <= ty) 
			{ // tx smallest, step in x
				if (i == iend) break;
				tx += deltatx;
				i += di;
			}
			else
			{ // ty smallest, step in y
				if (j == jend) break;
				ty += deltaty;
				j += dj;
			}
		}
		_pointOfIntersection = Grid::get().getTileCenter(i, j);
			
		return isSuccessful;
	}

	void drawFieldOfViewBeam(float angle, Orientation::Enum sourceOrientation, float targetX, const Vector &sourcePosition)
	{
		float angleSlope = tan(angle * sourceOrientation);
		float angleTargetY = angleSlope * targetX - angleSlope * sourcePosition.getX() + sourcePosition.getY();
		Graphics::get().drawLine(sourcePosition, Vector(targetX, angleTargetY), Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawFieldOfView(const Vector &sourcePosition, Orientation::Enum sourceOrientation) const
	{
		static const float SIGHT_BEAM_LENGTH = 1024.0f;
		float targetX = sourcePosition.getX() + SIGHT_BEAM_LENGTH * sourceOrientation;

		drawFieldOfViewBeam(_lowerAngle, sourceOrientation, targetX, sourcePosition);
		drawFieldOfViewBeam(_upperAngle, sourceOrientation, targetX, sourcePosition);
	}

	void Sight::drawDebugInfo(void) const
	{
		const Vector& sourcePosition = *(Vector*)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(Orientation::Enum*)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Vector& targetPosition = *(Vector*)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));

		drawFieldOfView(sourcePosition, sourceOrientation);
		if(_pointOfIntersection != Vector::Zero)
			Graphics::get().drawLine(sourcePosition, _pointOfIntersection, _isSeeing ? Color::Green : Color::Red);
	}
}