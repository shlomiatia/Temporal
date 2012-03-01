#include "Sight.h"
#include "StaticBody.h"
#include <Temporal/Game/QueryManager.h>
#include <Temporal/Graphics/Graphics.h>
#include <math.h>

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
			checkLineOfSight(true);
		}
	}

	void Sight::checkLineOfSight(bool drawDebugInfo) const
	{
		static const float SIGHT_ANGLE_LENGTH = 100.0f;

		const Vector& sourcePosition = *(const Vector* const)sendQueryMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(const Orientation::Enum* const)sendQueryMessageToOwner(Message(MessageID::GET_ORIENTATION));
		const Vector& targetPosition = *(const Vector* const)QueryManager::get().sendQueryMessageToEntity(0, Message(MessageID::GET_POSITION));

		if(drawDebugInfo)
		{
			float targetX = sourcePosition.getX() + SIGHT_ANGLE_LENGTH * sourceOrientation;

			float lowerAngleSlope = tan(_lowerAngle * sourceOrientation);
			float lowerAngleTargetY = lowerAngleSlope * targetX - lowerAngleSlope * sourcePosition.getX() + sourcePosition.getY();
			Graphics::get().drawLine(sourcePosition, Vector(targetX, lowerAngleTargetY));
			float upperAngleSlope = tan(_upperAngle * sourceOrientation);
			float upperAngleTargetY = upperAngleSlope * targetX - upperAngleSlope * sourcePosition.getX() + sourcePosition.getY();
			Graphics::get().drawLine(sourcePosition, Vector(targetX, upperAngleTargetY));
		}
		

		if((targetPosition.getX() - sourcePosition.getX()) * sourceOrientation < 0.0f)
			return;

		float slope = (targetPosition.getY() - sourcePosition.getY()) / (targetPosition.getX() - sourcePosition.getX());
		float angle = atan(slope);
		if(angle < _lowerAngle * sourceOrientation || angle > _upperAngle * sourceOrientation)
			return;

		bool rayCastSuccessful = rayCast(sourcePosition, targetPosition, drawDebugInfo);
		if(rayCastSuccessful && !drawDebugInfo)
			sendMessageToOwner(Message(MessageID::LINE_OF_SIGHT));
	}

	bool Sight::rayCast(const Vector& source, const Vector& destination, bool drawDebugInfo) const
	{
		float x1 = source.getX();
		float y1 = source.getY();
		float x2 = destination.getX();
		float y2 = destination.getY();
		
		int i = _staticBodiesIndex.getIndex(x1);
		int j = _staticBodiesIndex.getIndex(y1);

		// Determine end grid cell coordinates (iend, jend)
		int iend = _staticBodiesIndex.getIndex(x2);
		int jend = _staticBodiesIndex.getIndex(y2);

		// Determine in which primary direction to step
		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));

		const float tileSize = _staticBodiesIndex.getTileSize();

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

		// Main loop. Visits cells until last cell reached
		for (;;) 
		{
			std::vector<StaticBody*>* staticBodies = _staticBodiesIndex.get(i, j);
			Graphics::get().drawRect(Rect(i * _staticBodiesIndex.getTileSize(), j * _staticBodiesIndex.getTileSize(), 2.0f, 2.0f));
			if(staticBodies != NULL)
				return false;
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
		return true;
	}

}