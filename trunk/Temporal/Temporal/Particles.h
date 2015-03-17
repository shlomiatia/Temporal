#ifndef PARTICLES_H
#define PARTICLES_H

#include "Hash.h"
#include "Vector.h"
#include "Color.h"
#include "EntitySystem.h"
#include "Timer.h"
#include <memory>

namespace Temporal
{
	class Particle
	{
	public:
		Particle() : _position(Vector::Zero), _velocity(Vector::Zero), _angle(0.0f), _isAlive(true) {}
		void resetAge(float time = 0.0f) { _ageTimer.reset(time); }
		float getAge() const { return _ageTimer.getElapsedTime(); }
		const Vector& getPosition() const { return _position; }
		void setPosition(const Vector& position) { _position = position; }
		const Vector& getVelocity() const { return _velocity; }
		void setVelocity(const Vector& velocity) { _velocity = velocity; }
		bool isAlive() const { return _isAlive; }
		void setAlive(bool isAlive) { _isAlive = isAlive; } 
		float getAngle() const { return _angle; }
		void setAngle(float angle) { _angle = angle; }

		void update(float time);
	private:

		Vector _position;
		Vector _velocity;
		float _angle;
		Timer _ageTimer;
		bool _isAlive;

		Particle(const Particle&);
		Particle& operator=(const Particle&);
	};

	class SpriteSheet;

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter() : 
			_lifetime(0.0f), _birthThreshold(0.0f), _birthRadius(0.0f), _velocity(0.0f), _directionCenter(0.0f), _directionSize(0.0f), _birthIndex(0), _particles(0), _emit(false), _attachment(Hash::INVALID) {}
		~ParticleEmitter();

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const { return new ParticleEmitter(); }

		static const Hash TYPE;
	private:

		std::string _textureFile;
		std::string _spritesheetFile;
		std::shared_ptr<SpriteSheet> _spritesheet;
		
		float _lifetime;
		float _birthThreshold;
		float _birthRadius;
		float _velocity;
		float _directionCenter;
		float _directionSize;

		Timer _birthTimer;
		int _birthIndex;
		Particle* _particles;
		bool _emit;
		Hash _attachment;
		
		void init();
		void update(float framePeriod);
		void draw();
		int getLength();
		void emit(Vector emitterPosition, float emitterAngle, int length);

		friend class SerializationAccess;
	};
}

#endif