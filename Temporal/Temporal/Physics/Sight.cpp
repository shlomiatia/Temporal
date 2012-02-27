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
		static const float STEP = 10.0f;

		float x0 = source.getX();
		float y0 = source.getY();
		float x1 = destination.getX();
		float y1 = destination.getY();

		float dx = abs(x1-x0);
		float dy = abs(y1 - y0);
		float sx = x0 < x1 ? 1.0f : -1.0f;
		float sy = y0 < y1 ? 1.0f : -1.0f;
		float err = dx - dy;
		float e2;
		while(true)
		{
			e2 = 2.0f * err;
			if(e2 > -dy)
			{
				err -= dy;
				x0 += sx * STEP;
			}
			if(e2 < dx)
			{
				err += dx;
				y0 += sy * STEP;
			}
			if(drawDebugInfo)
				Graphics::get().drawRect(Rect(x0, y0, 2.0f, 2.0f));
			if((x1 - x0) * sx <= STEP && (y1 - y0) * sy <= STEP)
				return true;
			ComponentOfTypeIteraor iterator = QueryManager::get().getComponentOfTypeIteraor(ComponentType::STATIC_BODY);
			while(iterator.next())
			{
				StaticBody& staticBody = (StaticBody&)iterator.current();
				Rect staticBodyBounds = staticBody.getBounds();
				if(staticBodyBounds.contains(x0, y0))
					return false;
			}
		}
	}

}