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
	class ParticleSample
	{
	public:
		explicit ParticleSample(float interpolation = 0.0f, const Vector& scale = Vector(1.0f, 1.0f), Color color = Color::White, float rotation = 0.0f) :
			_interpolation(interpolation), _scale(scale), _color(color), _rotation(rotation) {}

		void setInterpolation(float interpolation) { _interpolation = interpolation; }
		float getInterpolation() const { return _interpolation; }
		void setRotation(float rotation) { _rotation = rotation; }
		float getRotation() const { return _rotation; }
		void setScale(const Vector& scale) { _scale = scale; }
		const Vector& getScale() const { return _scale; }
		void setColor(const Color& color) { _color = color; }
		const Color& getColor() const { return _color; }

		ParticleSample* clone() const { return new ParticleSample(_interpolation, _scale, _color, _rotation); }

	private:
		float _interpolation;
		Vector _scale;
		Color _color;
		float _rotation;

		ParticleSample(const ParticleSample&);
		ParticleSample& operator=(const ParticleSample&);

		friend class SerializationAccess;
	};

	typedef std::vector<ParticleSample*> ParticleSampleList;
	typedef ParticleSampleList::const_iterator ParticleSampleIterator;

	class Particle
	{
	public:
		Particle() : _position(Vector::Zero), _velocity(Vector::Zero), _rotation(0.0f), _isAlive(true), _scale(Vector(1.0f, 1.0f)), _color(Color::White) {}
		void resetAge(float time = 0.0f) { _ageTimer.reset(time); }
		float getAge() const { return _ageTimer.getElapsedTime(); }
		const Vector& getPosition() const { return _position; }
		void setPosition(const Vector& position) { _position = position; }
		const Vector& getVelocity() const { return _velocity; }
		void setVelocity(const Vector& velocity) { _velocity = velocity; }
		bool isAlive() const { return _isAlive; }
		void setAlive(bool isAlive) { _isAlive = isAlive; } 
		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }
		const Vector& getScale() const { return _scale; }
		void setScale(const Vector& scale) { _scale = scale; }
		const Color& getColor() const { return _color; }
		void setColor(const Color& color) { _color = color; }

		void update(float time, float gravity);
	private:

		Vector _position;
		Vector _velocity;
		float _rotation;
		Vector _scale;
		Color _color;
		Timer _ageTimer;
		bool _isAlive;

		Particle(const Particle&);
		Particle& operator=(const Particle&);
	};

	class SpriteSheet;

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter(const char* textureFile = "", const char* spritesheetFile = "", float lifetime = 0.0f, int size = 1, float birthRadius = 0.0f, float velocity = 0.0f, 
			float directionCenter = 0.0f, float directionSize = 0.0f, float minScale = 1.0f, float maxScale = 1.0f,float gravity = 0.0f/*, Hash attachment = Hash::INVALID*/) :
			_textureFile(textureFile), _spritesheetFile(spritesheetFile), _lifetime(lifetime), _size(size), _birthRadius(birthRadius), _velocity(velocity), _directionCenter(directionCenter),
			_directionSize(directionSize), _minScale(minScale), _maxScale(maxScale), _gravity(gravity), _birthIndex(0), _particles(0) /*, _attachment(attachment)*/{}
		~ParticleEmitter();

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		Component* clone() const 
			{ return new ParticleEmitter(_textureFile.c_str(), _spritesheetFile.c_str(), _lifetime, _size, _birthRadius, _velocity, _directionCenter, _directionSize, _minScale, _maxScale, _gravity /*, _attachment*/); }

		static const Hash TYPE;
	private:

		std::string _textureFile;
		std::string _spritesheetFile;
		std::shared_ptr<SpriteSheet> _spritesheet;
		
		float _lifetime;
		float _birthRadius;
		float _velocity;
		float _directionCenter;
		float _directionSize;
		float _minScale;
		float _maxScale;
		float _gravity;
		int _size;
		// Hash _attachment;

		Timer _birthTimer;
		int _birthIndex;
		Particle* _particles;

		ParticleSampleList _particleSamples;
		
		void init();
		void update(float framePeriod);
		void draw();
		void emit(const Vector& position, float age);
		void updateParticle(Particle& particle);

		friend class SerializationAccess;
	};
}

#endif