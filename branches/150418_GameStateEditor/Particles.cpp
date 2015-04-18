#include "Particles.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Math.h"
#include "Texture.h"
#include "MessageUtils.h"
#include "Layer.h"
#include "ResourceManager.h"
#include "SpriteSheet.h"
#include "SceneNode.h"
#include "Grid.h"
#include "Fixture.h"
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

	int ParticleEmitter::getLength() { return _birthThreshold == 0 ? 1 : static_cast<int>(_lifetime / _birthThreshold); }
	
	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			srand(Time::now() * 1000.0f);
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
		else if (message.getID() == MessageID::EMIT_PARTICLE)
		{
			_emit = true;
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
		Vector emitterPosition = getPosition(*this);;
		float emitterAngle = 0.0f;
		Side::Enum side =  getOrientation(*this);
		if(_attachment != Hash::INVALID)
		{
			SceneNode& root = *static_cast<SceneNode*>(raiseMessage(Message(MessageID::GET_ROOT_SCENE_NODE)));
			const SceneNode& node = *root.get(_attachment);
			Matrix nodeMatrix = node.getGlobalMatrix();
			Vector nodeTranslation = nodeMatrix.getTranslation();
			emitterAngle = nodeMatrix.getAngle();
			if(side == Side::LEFT)
				emitterAngle = AngleUtils::radian().mirror(emitterAngle);
			nodeTranslation.setX(nodeTranslation.getX() * side);
			emitterPosition += nodeTranslation;
		}
		
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
				RayCastResult result;
				if(getEntity().getManager().getGameState().getGrid().cast(particle.getPosition(), particle.getVelocity().normalize(), result) &&
					(result.getPoint() - particle.getPosition()).getLength() < particle.getVelocity().getLength() * framePeriod)
				{
					getEntity().getManager().sendMessageToEntity(result.getFixture().getEntityId(), Message(MessageID::DIE));
					particle.resetAge(_lifetime);
				}
				
			}
		}
		_birthTimer.update(framePeriod);
		float timeSinceLastBirth = _birthTimer.getElapsedTime();
		if(_birthThreshold > 0.0f && timeSinceLastBirth > _birthThreshold)
		{
			_birthTimer.reset();
			int bornParticles = static_cast<int>(timeSinceLastBirth / _birthThreshold);
			for(int i = 0; i < bornParticles; ++i)
			{
				emit(emitterPosition, emitterAngle, length);
			}
		}
		if(_emit)
		{
			emit(emitterPosition, emitterAngle, length);
			_emit = false;
		}
	}

	void ParticleEmitter::emit(Vector emitterPosition, float emitterAngle, int length)
	{
		float angle = randomF(1000) * _directionSize + _directionCenter + emitterAngle - _directionSize / 2.0f;
		Vector position = emitterPosition + Vector(-_birthRadius + randomF(1000) * _birthRadius * 2.0f, -_birthRadius + randomF(1000) * _birthRadius * 2.0f);
		Vector velocity = Vector(_velocity * cos(angle), _velocity * sin(angle));
		_particles[_birthIndex].setAlive(true);
		_particles[_birthIndex].setPosition(position);
		_particles[_birthIndex].setVelocity(velocity);
		_particles[_birthIndex].setAngle(emitterAngle);
		_particles[_birthIndex].resetAge();
		_birthIndex = (_birthIndex + 1) % length;
	}

	void ParticleEmitter::draw()
	{
		Side::Enum side =  getOrientation(*this);
		int length = getLength();
		for(int i = 0; i < length; ++i)
		{
			const Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				Graphics::get().getSpriteBatch().add(&_spritesheet->getTexture(), particle.getPosition(), AABB::Zero, Color::White, particle.getAngle());
			}
		}
	}
}