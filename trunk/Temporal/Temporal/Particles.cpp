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
	const Hash ParticleEmitter::TYPE = Hash("particle-emitter");

	void Particle::update(float time)
	{
		_ageTimer.update(time);
		_position += _velocity * time;
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete[] _particles;
	}

	int ParticleEmitter::getLength() { return static_cast<int>(_lifetime / _birthThreshold); }
	
	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			init();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
		else if(message.getID() == MessageID::DRAW)
		{
			draw();
		}
	}

	void ParticleEmitter::init()
	{
		getEntity().getManager().getGameState().getLayersManager().addSprite(LayerType::PARTICLES, this);
		int length = getLength();
		_particles = new Particle[length];
		if(!_spritesheetFile.empty())
			_spritesheet = ResourceManager::get().getSpritesheet(_spritesheetFile.c_str());
		else
			_spritesheet = ResourceManager::get().getSingleTextureSpritesheet(_textureFile.c_str());
	}

	int random(int max, int min = 0)
	{
		return rand() % max + min;
	}

	float randomF(int max)
	{
		return static_cast<float>(random(max)) / static_cast<float>(max);
	}

	void ParticleEmitter::update(float framePeriod)
	{
		int length = getLength();
		for(int i = 0; i < length; ++i)
		{
			Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				particle.update(framePeriod);
				if(particle.getAge() > _lifetime)
				{
					particle.setAlive(false);	
				}
			}
		}
		_birthTimer.update(framePeriod);
		float timeSinceLastBirth = _birthTimer.getElapsedTime();
		if(timeSinceLastBirth > _birthThreshold)
		{
			_birthTimer.reset();
			const Vector& emitterPosition = *static_cast<const Vector*>(raiseMessage(Message(MessageID::GET_POSITION)));

			int bornParticles = static_cast<int>(timeSinceLastBirth / _birthThreshold);
			for(int i = 0; i < bornParticles; ++i)
			{
				float angle = randomF(1000) * _directionSize + _directionCenter - _directionSize / 2.0f;
				Vector position = emitterPosition + Vector(-_birthRadius + randomF(1000) * _birthRadius * 2.0f, -_birthRadius + randomF(1000) * _birthRadius * 2.0f);
				Vector velocity = Vector(_velocity * cos(angle), _velocity * sin(angle));
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
		for(int i = 0; i < length; ++i)
		{
			const Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				Graphics::get().getSpriteBatch().add(&_spritesheet->getTexture(), particle.getPosition());
			}
		}
	}
}