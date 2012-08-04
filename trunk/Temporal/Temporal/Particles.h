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
			: _position(Point::Zero), _movement(Vector::Zero), _ageTimer(), _isAlive(true) 
		{
		}
		void resetAge() { _ageTimer.reset(); }
		float getAge() const { return _ageTimer.getElapsedTimeInMillis(); }
		const Point& getPosition() const { return _position; }
		void setPosition(const Point& position) { _position = position; }
		void setMovement(const Vector& movement) { _movement = movement; }
		void setAlive(bool isAlive) { _isAlive = isAlive; } 
		bool isAlive() const { return _isAlive; }

		void update(float time);
	private:
		Point _position;
		Vector _movement;
		Timer _ageTimer;
		bool _isAlive;

		Particle(const Particle&);
		Particle& operator=(const Particle&);
	};

	class Texture;

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter(const Texture* texture, float lifetimeInMillis, int birthsPerSecond);
		~ParticleEmitter();

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		const float LIFETIME_IN_MILLIS;
		const float BIRTH_THRESHOLD_IN_MILLIS;
		const Texture* _texture;

		Timer _birthTimer;
		int _birthIndex;
		Particle* _particles;
		float* _vertices;
		float* _texCoords;
		
		int getLength() const;
	};
}

#endif