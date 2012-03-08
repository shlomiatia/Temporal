#include "Sight.h"
#include "StaticBody.h"
#include "Grid.h"
#include <Temporal/Game/EntitiesManager.h>
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
		const Vector& sourcePosition = *(const Vector* const)sendMessageToOwner(Message(MessageID::GET_POSITION));
		Orientation::Enum sourceOrientation = *(const Orientation::Enum* const)sendMessageToOwner(Message(MessageID::GET_ORIENTATION));
		// TODO: Who're you watching? SLOTH!
		const Vector& targetPosition = *(const Vector* const)EntitiesManager::get().sendMessageToEntity(0, Message(MessageID::GET_POSITION));

		if(drawDebugInfo)
			drawLineOfSight(sourcePosition, sourceOrientation);

		// Check orientation
		if((targetPosition.getX() - sourcePosition.getX()) * sourceOrientation < 0.0f)
			return;

		// Check field of view
		// TODO: Test against top and bottom SLOTH!
		// TODO: Eyes SLOTH!
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

			// TODO: Compare with specific line SLOTH!
			std::vector<const StaticBody* const>* staticBodies = Grid::get().getTile(i, j);
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
		if(drawDebugInfo)
			Graphics::get().drawLine(source, Grid::get().getTileCenter(i, j), isSuccessful ? Color::Green : Color::Red);
		return isSuccessful;
	}

	void drawLineOfSightLine(float angle, Orientation::Enum sourceOrientation, float targetX, const Vector &sourcePosition)
	{
		float angleSlope = tan(angle * sourceOrientation);
		float angleTargetY = angleSlope * targetX - angleSlope * sourcePosition.getX() + sourcePosition.getY();
		Graphics::get().drawLine(sourcePosition, Vector(targetX, angleTargetY), Color(0.0f, 1.0f, 1.0f, 0.3f));
	}

	void Sight::drawLineOfSight(const Vector &sourcePosition, Orientation::Enum sourceOrientation) const
	{
		static const float SIGHT_ANGLE_LENGTH = 1024.0f;
		float targetX = sourcePosition.getX() + SIGHT_ANGLE_LENGTH * sourceOrientation;

		drawLineOfSightLine(_lowerAngle, sourceOrientation, targetX, sourcePosition);
		drawLineOfSightLine(_upperAngle, sourceOrientation, targetX, sourcePosition);
	}
}