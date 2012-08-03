#include "Particles.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Math.h"
#include <cstdlib>
#include <SDL_opengl.h>


namespace Temporal
{
	void Particle::update(float time)
	{
		_age.update(time);
		_position += _movement;
	}

	ParticleEmitter::ParticleEmitter(float deathAge, int birthRate)
			: DEATH_AGE(deathAge), BIRTH_RATE(birthRate), _particles(new Particle[getLength()]), _vertices(new float[getLength()*8]), _birthIndex(0) 
	{
	}

	int ParticleEmitter::getLength() const
	{
		return static_cast<int>(DEATH_AGE * BIRTH_RATE / 1000.0f);
	}
	
	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = *static_cast<float*>(message.getParam());
			int length = getLength();
			for(int i = 0; i < length; ++i)
			{
				Particle& particle = _particles[i];
				if(particle.isAlive())
				{
					particle.update(framePeriodInMillis);
					if(particle.getAge() > DEATH_AGE)
					{
						particle.setAlive(false);	
					}
				}
			}
			const Point& position = *(const Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));
			int bornParticles = static_cast<int>(BIRTH_RATE * framePeriodInMillis / 1000.0f);
			for(int i = 0; i < bornParticles; ++i)
			{
				float angle = (rand() % 1000) * 2 * PI / 1000.0f;
				Vector movement = Vector(cos(angle), sin(angle));
				_particles[_birthIndex].setAlive(true);
				_particles[_birthIndex].setPosition(position);
				_particles[_birthIndex].setMovement(movement);
				_particles[_birthIndex].resetAge();
				_birthIndex = (_birthIndex + 1) % length;
			}
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(layer == VisualLayer::NPC)
			{
				int length = getLength();
				glBindTexture(GL_TEXTURE_2D, 0);
				glColor4f(1.0f, 0.0f, 0.0f, 0.1f);
				float PARTICLE_SIZE = 1.0f;
				int j = 8;
				for(int i = 0; i < length; ++i)
				{
					const Particle& particle = _particles[i];
					if(particle.isAlive())
					{
						_vertices[i*j] = particle.getPosition().getX() - PARTICLE_SIZE;
						_vertices[i*j+1] = particle.getPosition().getY() - PARTICLE_SIZE;
						_vertices[i*j+2] = particle.getPosition().getX() - PARTICLE_SIZE;
						_vertices[i*j+3] = particle.getPosition().getY() + PARTICLE_SIZE;
						_vertices[i*j+4] = particle.getPosition().getX() + PARTICLE_SIZE;
						_vertices[i*j+5] = particle.getPosition().getY() + PARTICLE_SIZE;
						_vertices[i*j+6] = particle.getPosition().getX() + PARTICLE_SIZE;
						_vertices[i*j+7] = particle.getPosition().getY() - PARTICLE_SIZE;
					}
				}
				glEnableClientState(GL_VERTEX_ARRAY);
 
				glVertexPointer(2, GL_FLOAT, 0, _vertices);
 
				glDrawArrays(GL_QUADS, 0, length * 4);
 
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
}