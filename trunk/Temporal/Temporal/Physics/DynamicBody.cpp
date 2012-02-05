#include "DynamicBody.h"
#include "StaticBody.h"
#include <Temporal/Game/World.h>
#include <Temporal/Graphics/Graphics.h>
#include <algorithm>

namespace Temporal
{
	Direction::Enum calculateCollision(const Rect& boundsA, Orientation::Enum orientationA, const Rect& boundsB)
	{
		Direction::Enum collision(Direction::NONE);

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

	float correctCollisionInAxis(float force, float minDynamic, float maxDynamic, float minStatic, float maxStatic)
	{
		if(force > 0 && maxDynamic <= minStatic)
			force = std::min(force, minStatic - maxDynamic);
		else if(force < 0 && minDynamic >= maxStatic)
			force = std::max(force, maxStatic - minDynamic);
		return force;
	}

	DynamicBody::~DynamicBody(void) 
	{ 
		for(std::vector<Sensor* const>::iterator i = _sensors.begin(); i != _sensors.end(); ++i)
			delete *i;
	}

	Orientation::Enum DynamicBody::getOrientation(void) const
	{
		Orientation::Enum orientation = sendQueryMessageToOwner<Orientation::Enum>(MessageID::GET_ORIENTATION);
		return orientation;
	}

	void DynamicBody::applyGravity(void)
	{
		if(_gravityEnabled)
		{
			float y = _force.getY();
			y -= 1.0f;
			if(y < -20.0f)
				y = -20.0f;
			_force.setY(y);
		}
	}

	void DynamicBody::applyForce(void)
	{
		Vector position = sendQueryMessageToOwner<Vector>(MessageID::GET_POSITION);
		position += _force;
		sendMessageToOwner(Message(MessageID::SET_POSITION, &position));
	}

	void DynamicBody::add(Sensor* const sensor)
	{
		_sensors.push_back(sensor);
	}

	void DynamicBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		if(message.getID() == MessageID::GET_FORCE)
		{
			message.setParam(&_force);
		}
		else if(message.getID() == MessageID::SET_FORCE)
		{
			const Vector& force = message.getParam<Vector>();
			_force = Vector(force.getX() * getOrientation(), force.getY()); 
		}
		else if(message.getID() == MessageID::SET_GRAVITY)
		{
			_gravityEnabled = message.getParam<bool>();
		}
		else if(message.getID() == MessageID::RAY_CAST)
		{
			const Vector& destination = message.getParam<Vector>();
			if(rayCast(destination))
			{
				sendMessageToOwner(Message(MessageID::RAY_CAST_SUCCESS));
			}
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			update();
		}
		else if(message.getID() == MessageID::DRAW)
		{
			for(std::vector<Sensor* const>::iterator i = _sensors.begin(); i != _sensors.end(); ++i)
			{
				const Sensor& sensor = **i;
				Graphics::get().drawRect(sensor.getBounds(), sensor.getSensedBody() != NULL ? Color::Green : Color::Red);
			}
		}
	}

	void DynamicBody::update(void)
	{
		applyGravity();
		ComponentOfTypeIteraor iterator = World::get().getComponentOfTypeIteraor(ComponentType::STATIC_BODY);
		while(iterator.next())
		{
			StaticBody& staticBody = (StaticBody&)iterator.current();
			correctCollision(staticBody);
		}
		applyForce();
		_collision = Direction::NONE;
		iterator.reset();
		while(iterator.next())
		{
			StaticBody& staticBody = (StaticBody&)iterator.current();
			detectCollision(staticBody);
		}

		sendMessageToOwner(Message(MessageID::BODY_COLLISION, &_collision));
		for(std::vector<Sensor* const>::iterator i = _sensors.begin(); i != _sensors.end(); ++i)
		{
			Sensor& sensor = **i;
			sensor.setSensedBody(NULL);
			const Rect& sensorBounds = sensor.getBounds();
			iterator.reset();
			while(iterator.next())
			{
				StaticBody& staticBody = (StaticBody&)iterator.current();
				Orientation::Enum sensorOwnerOrientation = sensor.getOwner().getOrientation();
				const Rect& staticBodyBounds = staticBody.getBounds();
				Direction::Enum collision = calculateCollision(sensorBounds, sensorOwnerOrientation, staticBodyBounds);
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
			const Body* const body = sensor.getSensedBody();
			if(body != NULL)
			{
				sendMessageToOwner(Message(MessageID::SENSOR_COLLISION, &sensor));
			}
		}
	}

	void DynamicBody::correctCollision(const Body& staticBody)
	{
		const Rect& staticBodyBounds(staticBody.getBounds());
		const Rect& dynamicBodyBounds(getBounds());
		const Rect& futureBounds = dynamicBodyBounds + _force;

		if(futureBounds.intersectsExclusive(staticBodyBounds))
		{
			// TODO: Correct smallest axis
			// TODO: Gradual test
			float x = correctCollisionInAxis(_force.getX(), dynamicBodyBounds.getLeft(), dynamicBodyBounds.getRight(), staticBodyBounds.getLeft(), staticBodyBounds.getRight());
			float y = correctCollisionInAxis(_force.getY(), dynamicBodyBounds.getBottom(), dynamicBodyBounds.getTop(), staticBodyBounds.getBottom(), staticBodyBounds.getTop());
			_force.setX(x);
			_force.setY(y);
		}
	}

	void DynamicBody::detectCollision( const Body& staticBody )
	{
		const Rect& dynamicBodyBounds = getBounds();
		Orientation::Enum dynamicBodyOrientation = getOrientation();
		const Rect& staticBodyBounds = staticBody.getBounds();
		Direction::Enum collision = calculateCollision(dynamicBodyBounds, dynamicBodyOrientation, staticBodyBounds);
		_collision = _collision | collision;
	}

	bool DynamicBody::rayCast(const Vector& destination) const
	{
		const Vector& position = sendQueryMessageToOwner<Vector>(MessageID::GET_POSITION);
		float x0 = position.getX();
		float y0 = position.getY();
		float x1 = destination.getX();
		float y1 = destination.getY();
		const float STEP = 10.0f;

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
			// TODO: Investigate
			if((x1 - x0) * sx <= 1.0f && (y1 - y0) * sy <= 1.0f)
				return true;
			ComponentOfTypeIteraor iterator = World::get().getComponentOfTypeIteraor(ComponentType::STATIC_BODY);
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