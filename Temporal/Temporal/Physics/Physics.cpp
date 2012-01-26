#include "Physics.h"
//#include <Temporal\Graphics\Graphics.h>
#include <math.h>
#include <algorithm>

namespace Temporal
{
	const float Physics::GRAVITY = 1.0;
	const float Physics::MAX_GRAVITY = -20.0;

	Direction::Type getCollision(const Rect& boundsA, Orientation::Type orientationA, const Rect& boundsB)
	{
		Direction::Type collision(Direction::NONE);

		if(boundsA.intersectsInclusive(boundsB))
		{
			float frontA = boundsA.getSide(orientationA);
			float frontB = boundsB.getSide(orientationA);
			float backA = boundsA.getOppositeSide(orientationA);
			float backB = boundsB.getOppositeSide(orientationA);
			bool overlapsX = boundsA.getLeft() < boundsB.getRight() && boundsA.getRight() > boundsB.getLeft();
			bool overlapsY = boundsA.getBottom() < boundsB.getTop() && boundsA.getTop() > boundsB.getBottom();
			if(boundsA.getTop() >= boundsB.getTop() && overlapsX) collision = collision | Direction::BOTTOM;
			if(boundsA.getBottom() <= boundsB.getBottom() && overlapsX) collision = collision | Direction::TOP;
			if((frontA - frontB) * orientationA >= 0 && overlapsY) collision = collision | Direction::BACK;
			if((backB - backA) * orientationA >= 0 && overlapsY) collision = collision | Direction::FRONT;
		}

		return collision;
	}

	void correctCollision(Body& staticBody, DynamicBody& dynamicBody)
	{
		const Rect& staticBodyBounds = staticBody.getBounds();
		dynamicBody.correctCollision(staticBodyBounds);
	}

	void detectCollision(Body& staticBody, DynamicBody& dynamicBody)
	{
		const Rect& dynamicBodyBounds = dynamicBody.getBounds();
		Orientation::Type dynamicBodyOrientation = dynamicBody.getOrientation();
		const Rect& staticBodyBounds = staticBody.getBounds();
		Direction::Type collision = getCollision(dynamicBodyBounds, dynamicBodyOrientation, staticBodyBounds);
		dynamicBody.addCollision(collision);
	}

	bool Physics::rayCast(const Body& source, const Body& destination) const
	{
		float x0 = source.getBounds().getCenterX();
		float y0 = source.getBounds().getCenterY();
		float x1 = destination.getBounds().getCenterX();
		float y1 = destination.getBounds().getCenterY();
        const float STEP = 15.0f;
        
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
			//Graphics::get().drawRect(Rect(x0, y0, 3.0f, 3.0f), Color::Cyan);
			if((x1 - x0) * sx <= 1.0f && (y1 - y0) * sy <= 1.0f)
                return true;
			for(int staticBodiesIndex = 0; staticBodiesIndex < _staticBodiesCount; ++staticBodiesIndex)
			{
				Body& staticBody = *_staticBodies[staticBodiesIndex];
				if(&staticBody != &source && &staticBody != &destination && staticBody.getBounds().contains(x0, y0))
					return false;
			}
        }
	}

	void Physics::processCollisions(DynamicBody& dynamicBody)
	{
		dynamicBody.applyGravity();
		for(int staticBodiesIndex = 0; staticBodiesIndex < _staticBodiesCount; ++staticBodiesIndex)
		{
			Body& staticBody = *_staticBodies[staticBodiesIndex];
			correctCollision(staticBody, dynamicBody);
		}
		dynamicBody.applyForce();
		dynamicBody.clearCollision();
		for(int staticBodiesIndex = 0; staticBodiesIndex < _staticBodiesCount; ++staticBodiesIndex)
		{
			Body& staticBody = *_staticBodies[staticBodiesIndex];
			detectCollision(staticBody, dynamicBody);
		}
	}

	

	void Physics::processSensors(DynamicBody& dynamicBody)
	{
		for(int sensorIndex = 0; sensorIndex < dynamicBody._elementsCount; ++sensorIndex)
		{
			Sensor& sensor = *dynamicBody._elements[sensorIndex];
			sensor.setSensedBody(NULL);
			for(int staticBodiesIndex = 0; staticBodiesIndex < _staticBodiesCount; ++staticBodiesIndex)
			{
				Body& staticBody = *_staticBodies[staticBodiesIndex];
				const Rect& sensorBounds = sensor.getBounds();
				Orientation::Type sensorOwnerOrientation = sensor.getOwner().getOrientation();
				const Rect& staticBodyBounds = staticBody.getBounds();
				Direction::Type collision = getCollision(sensorBounds, sensorOwnerOrientation, staticBodyBounds);
				if(match(collision, sensor.getPositive(), sensor.getNegative()))
				{
					sensor.setSensedBody(&staticBody);
				}
				else if(collision != Direction::NONE)
				{
					sensor.setSensedBody(NULL);
					break;
				}
			}
		}
	}

	void Physics::update(void)
	{
		for(int dynamicBodiesIndex = 0; dynamicBodiesIndex < _dynamicBodiesCount; ++dynamicBodiesIndex)
		{
			DynamicBody& dynamicBody = *_dynamicBodies[dynamicBodiesIndex];
			processCollisions(dynamicBody);
		}
		for(int dynamicBodiesIndex = 0; dynamicBodiesIndex < _dynamicBodiesCount; ++dynamicBodiesIndex)
		{
			DynamicBody& dynamicBody = *_dynamicBodies[dynamicBodiesIndex];
			processSensors(dynamicBody);
		}
	}
}