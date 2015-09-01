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

#include "Mouse.h"

namespace Temporal
{
	const Hash ParticleEmitter::TYPE = Hash("particle-emitter");

	void Particle::update(float time, float gravity)
	{
		_ageTimer.update(time);
		_velocity += Vector(0.0f, gravity) * time;
		_position += _velocity * time;
	}

	ParticleEmitter::~ParticleEmitter()
	{
		delete[] _particles;
		for (ParticleSampleIterator i = _particleSamples.begin(); i != _particleSamples.end(); ++i)
			delete *i;
	}

	int ParticleEmitter::getLength() { return _birthThreshold == 0 ? 1 : static_cast<int>(_lifetime / _birthThreshold); }
	
	void ParticleEmitter::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			srand(Time::now() * 1000.0f);
			init();
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().getGameState().getLayersManager().removeSprite(LayerType::PARTICLES, this);
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

	void ParticleEmitter::updateParticle(Particle& particle)
	{
		float interpolation = particle.getAge() / _lifetime;
		for (ParticleSampleIterator j = _particleSamples.begin(); j != (_particleSamples.end() - 1); ++j)
		{
			const ParticleSample& current = **j;
			const ParticleSample& next = **(j + 1);
			if (interpolation >= current.getInterpolation() && interpolation <= next.getInterpolation())
			{
				float realInterpolation = (interpolation - current.getInterpolation()) / (next.getInterpolation() - current.getInterpolation());
				Color color(
					easeInOutBezier(realInterpolation, current.getColor().getR(), next.getColor().getR()),
					easeInOutBezier(realInterpolation, current.getColor().getG(), next.getColor().getG()),
					easeInOutBezier(realInterpolation, current.getColor().getB(), next.getColor().getB()),
					easeInOutBezier(realInterpolation, current.getColor().getA(), next.getColor().getA()));
				particle.setColor(color);

				if (_minScale == 0.0f && _maxScale == 0.0f)
				{
					Vector scale(
						easeInOutBezier(realInterpolation, current.getScale().getX(), next.getScale().getX()),
						easeInOutBezier(realInterpolation, current.getScale().getY(), next.getScale().getY()));
					particle.setScale(scale);
				}

				if (current.getRotation() != 0.0f || next.getRotation() != 0.0f)
				{
					float rotation = easeInOutBezier(realInterpolation, current.getRotation(), next.getRotation());
					particle.setRotation(rotation);
				}
				
				break;
			}
		}
	}

	void ParticleEmitter::update(float framePeriod)
	{
		Vector& vector = Mouse::get().getOffsetPosition();
		raiseMessage(Message(MessageID::SET_POSITION, &vector));
		int length = getLength();
		Vector emitterPosition = getPosition(*this);;
		/*float emitterAngle = 0.0f;
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
		}*/
		
		for(int i = 0; i < length; ++i)
		{
			Particle& particle = _particles[i];
			if(particle.isAlive())
			{
				particle.update(framePeriod, _gravity);
				if(particle.getAge() > _lifetime)
				{
					particle.setAlive(false);	
				}
				else
				{
					updateParticle(particle);
				}
			}
		}
		_birthTimer.update(framePeriod);
		float timeSinceLastBirth = _birthTimer.getElapsedTime();
		if(_birthThreshold > 0.0f && timeSinceLastBirth > _birthThreshold)
		{
			_birthTimer.reset();
			int bornParticles = static_cast<int>(timeSinceLastBirth / _birthThreshold);
			int lastParticleIndex = (length + _birthIndex - 1) % length;
			for(int i = 0; i < bornParticles; ++i)
			{
				float interpolation = static_cast<float>(i + 1) / static_cast <float>(bornParticles);
				Vector position(
					easeInOutBezier(interpolation, _particles[lastParticleIndex].getPosition().getX(), emitterPosition.getX()),
					easeInOutBezier(interpolation, _particles[lastParticleIndex].getPosition().getY(), emitterPosition.getY()));
				emit(position, framePeriod * interpolation);
			}
		}
 	}

	void ParticleEmitter::emit(const Vector& position, float age)
	{
		float angle = 0.0f;
		int length = getLength();
		float realAngle = randomF(1000) * _directionSize + _directionCenter + angle - _directionSize / 2.0f;
		Vector realPosition = position + Vector(-_birthRadius + randomF(1000) * _birthRadius * 2.0f, -_birthRadius + randomF(1000) * _birthRadius * 2.0f);
		Vector velocity = Vector(_velocity * cos(realAngle), _velocity * sin(realAngle));
		float scale = _minScale + randomF(1000) * (_maxScale - _minScale);
		Particle& particle = _particles[_birthIndex];
		particle.setAlive(true);
		particle.setPosition(realPosition);
		particle.setVelocity(velocity);
		particle.setRotation(realAngle);
		particle.setScale(Vector(1.0f, 1.0f) * scale);
		particle.resetAge(age);
		updateParticle(particle);
		_birthIndex = (_birthIndex + 1) % length;
	}

	void ParticleEmitter::draw()
	{
		Graphics::get().getSpriteBatch().end();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		Graphics::get().getSpriteBatch().begin();
		Side::Enum side =  getOrientation(*this);
		int length = getLength();
		for(int i = 0; i < length; ++i)
		{
			int j = (i + _birthIndex) % length;
			const Particle& particle = _particles[j];
			if(particle.isAlive())
			{
				Graphics::get().getSpriteBatch().add(&_spritesheet->getTexture(), particle.getPosition(), AABB::Zero, particle.getColor(), particle.getRotation(), Vector::Zero, particle.getScale());
			}
		}
		Graphics::get().getSpriteBatch().end();
		Graphics::get().getSpriteBatch().begin();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}