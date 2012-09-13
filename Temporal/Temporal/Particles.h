#ifndef PARTICLES_H
#define PARTICLES_H

#include "Hash.h"
#include "Vector.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Timer.h"

namespace Temporal
{
	class Particle
	{
	public:
		Particle() : _position(Vector::Zero), _velocity(Vector::Zero), _isAlive(true) {}
		void resetAge() { _ageTimer.reset(); }
		float getAge() const { return _ageTimer.getElapsedTime(); }
		const Vector& getPosition() const { return _position; }
		void setPosition(const Vector& position) { _position = position; }
		void setVelocity(const Vector& velocity) { _velocity = velocity; }
		bool isAlive() const { return _isAlive; }
		void setAlive(bool isAlive) { _isAlive = isAlive; } 

		void update(float time);
	private:
		Vector _position;
		Vector _velocity;
		Timer _ageTimer;
		bool _isAlive;

		Particle(const Particle&);
		Particle& operator=(const Particle&);
	};

	class SpriteSheet;

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter() : _lifetime(0.0f), _birthThreshold(0.0f), _particles(NULL), _vertices(NULL), _texCoords(NULL), _birthIndex(0),
			_spritesheetId(Hash::INVALID), _spritesheet(NULL), _birthRadius(0.0f), _velocity(0.0f), _directionCenter(0.0f), _directionSize(0.0f) {}
		~ParticleEmitter();

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);

		Component* clone() const { return new ParticleEmitter(); }

	private:
		float _lifetime;
		float _birthThreshold;
		Hash _spritesheetId;
		const SpriteSheet* _spritesheet;
		float _birthRadius;
		float _velocity;
		float _directionCenter;
		float _directionSize;

		Timer _birthTimer;
		int _birthIndex;
		Particle* _particles;
		float* _vertices;
		float* _texCoords;
		
		void init();
		void update(float framePeriod);
		void draw();
		int getLength();

		friend class SerializationAccess;
	};
}

#endif