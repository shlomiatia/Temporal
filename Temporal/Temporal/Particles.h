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
		Particle(Point position, Vector movement, Color color)
			: _position(position), _movement(movement), _color(color), _age() {}
		float getAge() { return _age.getElapsedTimeInMillis(); }

		void update(float time);
		void draw() const;
	private:
		Point _position;
		Vector _movement;
		Color _color;
		Timer _age;
	};

	typedef std::vector<Particle*> ParticleCollection;
	typedef ParticleCollection::const_iterator ParticleIterator;

	class ParticleEmitter : public Component
	{
	public:
		ParticleEmitter(float deathAge, int birthRate)
			: DEATH_AGE(deathAge), BIRTH_RATE(birthRate) {}

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }
		void handleMessage(Message& message);
	private:
		const float DEATH_AGE;
		const int BIRTH_RATE;

		ParticleCollection _particles;
	};
}

#endif