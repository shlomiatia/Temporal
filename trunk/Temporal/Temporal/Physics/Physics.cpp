#include "Physics.h"
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
			if((frontB - frontA) * orientationA >= 0 && overlapsY) collision = collision | Direction::FRONT;
			if((backA - backB) * orientationA >= 0 && overlapsY) collision = collision | Direction::BACK;
		}

		return collision;
	}

	bool Physics::rayCast(const Body& source, const Body& destination) const
	{
		float x0 = source.getBounds().getCenterX();
		float y0 = source.getBounds().getCenterY();
		float x1 = destination.getBounds().getCenterX();
		float y1 = destination.getBounds().getCenterY();
        const float STEP = 20.0f;
        
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
			if((x1 - x0) * sx <= 1.0f && (y1 - y0) * sy <= 1.0f)
                return true;
			for(int bodiesIndex = 0; bodiesIndex < _elementsCount; ++bodiesIndex)
			{
				Body& body = *_elements[bodiesIndex];
				if(!body.isDynamic())
				{
					if(&body != &source && &body != &destination && body.getBounds().contains(x0, y0))
						return false;
				}
			}
        }
	}

	void Physics::processBodies(bool isDynamic, void (Physics::*processBody)(Body&, void*), void* param)
	{
		for(int bodiesIndex = 0; bodiesIndex < _elementsCount; ++bodiesIndex)
		{
			Body& body = *_elements[bodiesIndex];
			if(body.isDynamic() == isDynamic)
			{
				(this->*processBody)(body, param);
			}
		}
	}

	void Physics::correctCollision(Body& staticBody, void* param)
	{
		Body& dynamicBody = *(Body*)param;
		const Rect& staticBodyBounds = staticBody.getBounds();
		dynamicBody.correctCollision(staticBodyBounds);
	}

	void Physics::detectCollision(Body& staticBody, void* param)
	{
		Body& dynamicBody = *(Body*)param;
		const Rect& dynamicBodyBounds = dynamicBody.getBounds();
		Orientation::Type dynamicBodyOrientation = dynamicBody.getOrientation();
		const Rect& staticBodyBounds = staticBody.getBounds();
		Direction::Type collision = getCollision(dynamicBodyBounds, dynamicBodyOrientation, staticBodyBounds);
		dynamicBody.addCollision(collision);
	}

	void Physics::processCollisions(Body& dynamicBody, void* param)
	{
		dynamicBody.applyGravity();
		processBodies(false, &Physics::correctCollision, &dynamicBody);
		dynamicBody.applyForce();

		dynamicBody.clearCollision();
		processBodies(false, &Physics::detectCollision, &dynamicBody);
	}

	void Physics::processSensor(Body& staticBody, void* param)
	{
		Sensor& sensor = *(Sensor*)param;
		const Rect& sensorBounds = sensor.getBounds();
		Orientation::Type sensorOwnerOrientation = sensor.getOwner().getOrientation();
		const Rect& staticBodyBounds = staticBody.getBounds();
		Direction::Type collision = getCollision(sensorBounds, sensorOwnerOrientation, staticBodyBounds);
		if(collision != Direction::NONE)
		{
			sensor.setCollision(&staticBody, collision);
		}
	}

	void Physics::processSensors(Body& dynamicBody, void* param)
	{
		for(int sensorIndex = 0; sensorIndex < dynamicBody._elementsCount; ++sensorIndex)
		{
			Sensor& sensor = *dynamicBody._elements[sensorIndex];
			sensor.clearCollision();
			processBodies(false, &Physics::processSensor, &sensor);
		}
	}

	void Physics::update(void)
	{
		processBodies(true, &Physics::processCollisions);
		processBodies(true, &Physics::processSensors);
	}
}