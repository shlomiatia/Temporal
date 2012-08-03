#ifndef PARTICLES_H
#define PARTICLES_H

#include "NumericPair.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Timer.h"

namespace Temporal
{
	class Particle
	{
	public:
		Particle()
			: _position(Point::Zero), _movement(Vector::Zero), _age(), _isAlive(true) 
		{
		}
		void resetAge() { _age.reset(); }
		float getAge() const { return _age.getElapsedTimeInMillis(); }
		const Point& getPosition() const { return _position; }
		void setPosition(const Point& position) { _position = position; }
		void setMovement(const Vector& movement) { _movement = movement; }
		void setAlive(bool isAlive) { _isAlive = isAlive; } 
		bool isAlive() const { return _isAlive; }

		void update(float time);
	private:
		Point _position;
		Vector _movement;
		Timer _age;
		bool _isAlive;

		Particle(const Particle&);
		Particle& operator=(const Particle&);
	};

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter(float deathAge, int birthRate);
		~ParticleEmitter() { delete[] _particles; delete[] _vertices; }

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		const float DEATH_AGE;
		const int BIRTH_RATE;

		int _birthIndex;
		Particle* _particles;
		float* _vertices;

		int getLength() const;
	};
}

#endif