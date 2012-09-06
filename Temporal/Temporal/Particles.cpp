#include "Particles.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Math.h"
#include "Texture.h"
#include "MessageUtils.h"
#include "Layer.h"
#include "ResourceManager.h"
#include "SpriteSheet.h"
#include <cstdlib>
#include <SDL_opengl.h>

namespace Temporal
{
	void Particle::update(float time)
	{
		_ageTimer.update(time);
		_position += _velocity * time/ 1000.0f;
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete[] _particles;
		delete[] _vertices;
		delete[] _texCoords;
	}

	int ParticleEmitter::getLength() { return static_cast<int>(_lifetimeInMillis / _birthThresholdInMillis); }
	
	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			init();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriodInMillis = getFloatParam(message.getParam());
			update(framePeriodInMillis);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
			if(layer == LayerType::PARTICLES)
			{
				draw();
			}
		}
	}

	void ParticleEmitter::init()
	{
		int length = getLength();
		_particles = new Particle[length];
		_vertices = new float[length*8];
		_texCoords = new float[length*8];
		if(_spritesheetId != Hash::INVALID)
			_spritesheet = ResourceManager::get().getSpritesheet(_spritesheetId);
		int j = 8;
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

	int random(int max, int min = 0)
	{
		return rand() % max + min;
	}

	float randomF(int max)
	{
		return static_cast<float>(random(max)) / static_cast<float>(max);
	}

	void ParticleEmitter::update(float framePeriodInMillis)
	{
		int length = getLength();
		for(int i = 0; i < length; ++i)
		{
			Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				particle.update(framePeriodInMillis);
				if(particle.getAge() > _lifetimeInMillis)
				{
					particle.setAlive(false);	
				}
			}
		}
		_birthTimer.update(framePeriodInMillis);
		float timeSinceLastBirth = _birthTimer.getElapsedTimeInMillis();
		if(timeSinceLastBirth > _birthThresholdInMillis)
		{
			_birthTimer.reset();
			const Vector& emitterPosition = *static_cast<const Vector*>(raiseMessage(Message(MessageID::GET_POSITION)));

			int bornParticles = static_cast<int>(timeSinceLastBirth / _birthThresholdInMillis);
			for(int i = 0; i < bornParticles; ++i)
			{
				float angle = randomF(1000) * _directionSize + _directionCenter - _directionSize / 2.0f;
				Vector position = emitterPosition + Vector(randomF(1000) * _birthRadius, randomF(1000) * _birthRadius);
				Vector velocity = Vector(_velocityPerSecond * cos(angle), _velocityPerSecond * sin(angle));
				_particles[_birthIndex].setAlive(true);
				_particles[_birthIndex].setPosition(position);
				_particles[_birthIndex].setVelocity(velocity);
				_particles[_birthIndex].resetAge();
				_birthIndex = (_birthIndex + 1) % length;
			}
		}
	}

	void ParticleEmitter::draw()
	{
		int length = getLength();
		glBindTexture(GL_TEXTURE_2D, _spritesheet->getTexture().getID());
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		Vector radius = _spritesheet->getTexture().getSize().toVector() / 2.0f;
		const int PARTICLE_VERTICES = 8;
		int vertexIndex = 0;
		for(int i = 0; i < length; ++i)
		{
			const Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				int startIndex = PARTICLE_VERTICES * vertexIndex;
				_vertices[startIndex] = particle.getPosition().getX() - radius.getX();
				_vertices[startIndex+1] = particle.getPosition().getY() - radius.getY();
				_vertices[startIndex+2] = particle.getPosition().getX() - radius.getX();
				_vertices[startIndex+3] = particle.getPosition().getY() + radius.getY();
				_vertices[startIndex+4] = particle.getPosition().getX() + radius.getX();
				_vertices[startIndex+5] = particle.getPosition().getY() + radius.getY();
				_vertices[startIndex+6] = particle.getPosition().getX() + radius.getX();
				_vertices[startIndex+7] = particle.getPosition().getY() - radius.getY();
				++vertexIndex;
			}
		}
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);	

		glVertexPointer(2, GL_FLOAT, 0, _vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, _texCoords);
 
		glDrawArrays(GL_QUADS, 0, vertexIndex * 4);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}