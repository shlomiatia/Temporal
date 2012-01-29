#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "Body.h"
#include "Sensor.h"

namespace Temporal
{
	class DynamicBody : public Body
	{
	public:
		static const int MAX_ELEMENTS = 10;
		Sensor* _elements[MAX_ELEMENTS];
		int _elementsCount;

		DynamicBody(const Vector& position, const Vector& size, Orientation::Type orientation = Orientation::RIGHT);
		virtual ~DynamicBody(void);

		Orientation::Type getOrientation(void) const { return _orientation; }
		const Vector getForce(void) const { return _force; }
		void setForce(const Vector& force) { _force = Vector(force.getX() * getOrientation(), force.getY()); }
		bool isGravityEnabled(void) const { return _gravityEnabled; }
		void setGravityEnabled(bool gravityEnabled) { _gravityEnabled = gravityEnabled; }
		const Sensor& getSensor(int id) const { return *_elements[id]; }
		float getFront(void) const { return getBounds().getSide(getOrientation()); }
		float getBack(void) const { return getBounds().getOppositeSide(getOrientation()); }
		Direction::Type getCollision(void) const { return _collision; }
		void clearCollision(void) { _collision = Direction::NONE; }
		void addCollision(Direction::Type collision) { _collision = _collision | collision; }
		bool isColliding(void) const { return getCollision() != Direction::NONE; }

		bool rayCast(const DynamicBody& other) const;

		void flipOrientation() { _orientation = _orientation == Orientation::LEFT ? Orientation::RIGHT : Orientation::LEFT; }
		void applyGravity(void);
		void correctCollision(const Rect& bodyBounds);
		void applyForce(void) { _bounds += _force; }
		void add(Sensor* const element) { _elements[_elementsCount++] = element; }

	private:
		Orientation::Type _orientation;
		Vector _force;
		bool _gravityEnabled;
		Direction::Type _collision;
	};
}