#pragma once

namespace Temporal { class Sensor; }

#include <Temporal\Base\Base.h>
#include "Sensor.h"

namespace Temporal
{
	class Body
	{
	public:
		// TODO: Dynamic
		static const int MAX_ELEMENTS = 10;
		Sensor* _elements[MAX_ELEMENTS];
		int _elementsCount;

		Body(bool isDynamic, const Vector& position, const Vector& size, Orientation::Type orientation = Orientation::RIGHT)
			: _isDynamic(isDynamic), _bounds(position, size), _orientation(orientation), _force(Vector::Zero), _gravityScale(1.0f), _elementsCount(0), _collision(Direction::NONE) {}
		~Body(void);

		bool isDynamic(void) const { return _isDynamic; }
		const Vector& getPosition(void) const { return _bounds.getCenter(); }
		const Rect& getBounds(void) const { return _bounds; }
		Orientation::Type getOrientation(void) const { return _orientation; }
		const Vector getForce(void) const { return _force; }
		void setForce(const Vector& force) { _force = Vector(force.getX() * getOrientation(), force.getY()); }
		float getGravityScale(void) const { return _gravityScale; }
		void setGravityScale(float gravityScale) { _gravityScale = gravityScale; }
		const Sensor& getSensor(int id) const { return *_elements[id]; }
		float getFront(void) const { return getBounds().getSide(getOrientation()); }
		float getBack(void) const { return getBounds().getOppositeSide(getOrientation()); }
		Direction::Type getCollision(void) const { return _collision; }
		void clearCollision(void) { _collision = Direction::NONE; }
		void addCollision(Direction::Type collision) { _collision = _collision | collision; }
		bool isColliding(void) const { return getCollision() != Direction::NONE; }

		void flipOrientation() { _orientation = _orientation == Orientation::LEFT ? Orientation::RIGHT : Orientation::LEFT; }
		void applyGravity(float gravity, float maxGravity);
		void correctCollision(const Rect& bodyBounds);
		void applyForce(void) { _bounds += _force; }
		void add(Sensor* element) { assert(_elementsCount < MAX_ELEMENTS); _elements[_elementsCount++] = element; }

		
	private:
		bool _isDynamic;
		Rect _bounds;
		Orientation::Type _orientation;
		Vector _force;
		float _gravityScale;
		Direction::Type _collision;

		Body(const Body&);
		Body& operator=(const Body&);
	};
}

