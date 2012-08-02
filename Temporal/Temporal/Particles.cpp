#include "Particles.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Math.h"
#include <cstdlib>

namespace Temporal
{
	void Particle::update(float time)
	{
		_age.update(time);
		_position += _movement;
	}

	void Particle::draw() const
	{
		Graphics::get().draw(AABB(_position, Size(1.0f, 1.0f)), _color);
	}

	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *static_cast<float*>(message.getParam());
			for(ParticleIterator i = _particles.begin(); i != _particles.end(); ++i)
			{
				Particle& particle = **i;
				particle.update(framePeriodInMillis);
				if(particle.getAge() > DEATH_AGE)
				{
					delete *i;
					i =_particles.erase(i);
				}
				if(i == _particles.end())
					break;
			}
			const Point& position = *(const Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			int bornParticles = static_cast<int>(BIRTH_RATE * framePeriodInMillis / 1000.0f);
			for(int i = 0; i < bornParticles; ++i)
			{
				float angle = (rand() % 1000) * 2 * PI / 1000.0f;
				Particle* particle = new Particle(position, Vector(cos(angle), sin(angle)), Color::Red);
				_particles.push_back(particle);
			}
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(layer == VisualLayer::NPC)
			{
				for(ParticleIterator i = _particles.begin(); i != _particles.end(); ++i)
				{
					(**i).draw();
				}
			}
		}
	}
}