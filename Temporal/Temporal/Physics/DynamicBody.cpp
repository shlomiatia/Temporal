#include "DynamicBody.h"
#include "StaticBody.h"
#include <Temporal/Game/World.h>
#include <Temporal/Graphics/Graphics.h>
#include <algorithm>

namespace Temporal
{
	Direction::Type calculateCollision(const Rect& boundsA, Orientation::Type orientationA, const Rect& boundsB)
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

	Orientation::Type DynamicBody::getOrientation(void) const
	{
		Message getPosition(MessageID::GET_ORIENTATION);
		sendMessage(getPosition);
		Orientation::Type orientation = getPosition.getParam<Orientation::Type>();
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
		Vector position(getBounds().getCenter());
		position += _force;
		sendMessage(Message(MessageID::SET_POSITION, &position));
	}

	void DynamicBody::add(Sensor* const sensor)
	{
		_sensors.push_back(sensor);
	}

	void DynamicBody::handleMessage(Message& message)
	{
		Body::handleMessage(message);
		switch(message.getID())
		{
			case(MessageID::GET_FORCE):
			{
				message.setParam(&_force);
				break;
			}
			case(MessageID::SET_FORCE):
			{
				const Vector& force = message.getParam<Vector>();
				_force = Vector(force.getX() * getOrientation(), force.getY()); 
				break;
			}
			case(MessageID::SET_GRAVITY):
			{
				_gravityEnabled = message.getParam<bool>();
				break;
			}
			case(MessageID::UPDATE):
			{
				update();
				break;
			}
			case(MessageID::DRAW):
			{
				for(std::vector<Sensor* const>::iterator i = _sensors.begin(); i != _sensors.end(); ++i)
				{
					const Sensor& sensor = **i;
					Graphics::get().drawRect(sensor.getBounds(), sensor.getSensedBody() != NULL ? Color::Green : Color::Red);
				}
				break;
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

		sendMessage(Message(MessageID::BODY_COLLISION, &_collision));
		for(std::vector<Sensor* const>::iterator i = _sensors.begin(); i != _sensors.end(); ++i)
		{
			Sensor& sensor = **i;
			sensor.setSensedBody(NULL);
			const Rect& sensorBounds = sensor.getBounds();
			iterator.reset();
			while(iterator.next())
			{
				StaticBody& staticBody = (StaticBody&)iterator.current();
				Orientation::Type sensorOwnerOrientation = sensor.getOwner().getOrientation();
				const Rect& staticBodyBounds = staticBody.getBounds();
				Direction::Type collision = calculateCollision(sensorBounds, sensorOwnerOrientation, staticBodyBounds);
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
				sendMessage(Message(MessageID::SENSOR_COLLISION, &sensor));
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
		Orientation::Type dynamicBodyOrientation = getOrientation();
		const Rect& staticBodyBounds = staticBody.getBounds();
		Direction::Type collision = calculateCollision(dynamicBodyBounds, dynamicBodyOrientation, staticBodyBounds);
		_collision = _collision | collision;
	}
}