#include "Particles.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Math.h"
#include "Texture.h"
#include <cstdlib>
#include <SDL_opengl.h>


namespace Temporal
{
	void Particle::update(float time)
	{
		_ageTimer.update(time);
		_position += _movement * time / 1000.0f;
	}

	ParticleEmitter::ParticleEmitter(const Texture* texture, float deathAge, int births) :
		DEATH_AGE(deathAge),
		BIRTH_RATE(1000.0f / births),
		_particles(new Particle[getLength()]),
		_vertices(new float[getLength()*8]),
		_texCoords(new float[getLength()*8]),
		_birthIndex(0),
		_texture(texture) 
	{
		int j = 8;
		int length = getLength();
		for(int i = 0; i < length; ++i)
		{
			_texCoords[i*j] = 0.0f;
			_texCoords[i*j+1] = 1.0f;
			_texCoords[i*j+2] = 0.0f;
			_texCoords[i*j+3] = 0.0f;
			_texCoords[i*j+4] = 1.0f;
			_texCoords[i*j+5] = 0.0f;
			_texCoords[i*j+6] = 1.0f;
			_texCoords[i*j+7] = 1.0f;
		}
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete[] _particles;
		delete[] _vertices;
		delete[] _texCoords;
		delete _texture; 
	}

	int ParticleEmitter::getLength() const
	{
		return static_cast<int>(DEATH_AGE / BIRTH_RATE);
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
			_birthTimer.update(framePeriodInMillis);
			float timeSinceLastBirth = _birthTimer.getElapsedTimeInMillis();
			if(timeSinceLastBirth > BIRTH_RATE)
			{
				_birthTimer.reset();
				const Point& position = *(const Point*)sendMessageToOwner(Message(MessageID::GET_POSITION));

				int bornParticles = static_cast<int>(timeSinceLastBirth / BIRTH_RATE);
				for(int i = 0; i < bornParticles; ++i)
				{
					float angle = (rand() % 1000) * ANGLE_60_IN_RADIANS / 1000.0f + ANGLE_60_IN_RADIANS;
					Vector movement = Vector(10.0f * cos(angle), 10.0f * sin(angle));
					_particles[_birthIndex].setAlive(true);
					_particles[_birthIndex].setPosition(position);
					_particles[_birthIndex].setMovement(movement);
					_particles[_birthIndex].resetAge();
					_birthIndex = (_birthIndex + 1) % length;
				}
			}
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(layer == VisualLayer::NPC)
			{
				int length = getLength();
				glBindTexture(GL_TEXTURE_2D, _texture->getID());
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				float PARTICLE_SIZE = 8.0f;
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
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);	

				glVertexPointer(2, GL_FLOAT, 0, _vertices);
				glTexCoordPointer(2, GL_FLOAT, 0, _texCoords);
 
				glDrawArrays(GL_QUADS, 0, length * 4);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
}