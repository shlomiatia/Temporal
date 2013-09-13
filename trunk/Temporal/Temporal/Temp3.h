#include "Hash.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Animation.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Math.h"

#include <sstream>
#include "Graphics.h"

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero), _translation(true), _startTime(0.0f) {}

		Hash getType() const { return Hash::INVALID; }

		void handleArrows(const Vector& vector)
		{
			Sample& sample = (**_sample);
			if(_translation)
			{
				sample.setTranslation(sample.getTranslation() + vector);
			}
			else
			{
				float angle = sample.getRotation() + vector.getX() + vector.getY();
				if(abs(angle) > 180.0f)
				{
					angle = (angle > 0.0f ? -360.0f : 360.0f) + angle;
				}
				sample.setRotation(angle);
			}
		}

		void setSample()
		{
			for(_sample = _sceneNode->second->getSamples().begin(); _sample != _sceneNode->second->getSamples().end() && (**_sample).getStartTime() <= _startTime; ++_sample);
			if(_sample != _sceneNode->second->getSamples().begin())
				--_sample;
		}

		void setStartTime()
		{
			_startTime = (**_sample).getStartTime();
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_startTime));
		}

		void updateEdit()
		{
			if(Mouse::get().isStartClicking(MouseButton::LEFT))
			{
				_offset = Mouse::get().getPosition() - (**_sample).getTranslation();
				_translation = true;
			}
			else if(Mouse::get().isClicking(MouseButton::LEFT))
			{
				(**_sample).setTranslation(Mouse::get().getPosition() - _offset);
				_animation->second->init();
			}
			else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
			{
				_offset = Mouse::get().getPosition() - (**_sample).getTranslation();
				_translation = false;
			}
			else if(Mouse::get().isClicking(MouseButton::RIGHT))
			{
				const Vector vector = Mouse::get().getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				(**_sample).setRotation(rotation);
				_animation->second->init();
			}
			else if(Keyboard::get().isPressing(Key::UP))
			{
				handleArrows(Vector(0.0f, 1.0f));
			}
			else if(Keyboard::get().isPressing(Key::DOWN))
			{
				handleArrows(Vector(0.0f, -1.0f));
			}
			else if(Keyboard::get().isPressing(Key::LEFT))
			{
				handleArrows(Vector(-1.0f, 0.0f));
			}
			else if(Keyboard::get().isPressing(Key::RIGHT))
			{
				handleArrows(Vector(1.0f, 0.0f));
			}
		}

		void update()
		{
			if((**_sample).getStartTime() == _startTime)
			{
				updateEdit();
			}
			if(Keyboard::get().isStartPressing(Key::SPACE))
			{
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::TOGGLE_ANIMATION));
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_startTime));
			}
			else if(Keyboard::get().isStartPressing(Key::W))
			{
				_sceneNode++;
				if(_sceneNode == _animation->second->getSampleSets().end())
					_sceneNode = _animation->second->getSampleSets().begin();
				setSample();

			}
			else if(Keyboard::get().isStartPressing(Key::S))
			{
				if(_sceneNode == _animation->second->getSampleSets().begin())
					_sceneNode = _animation->second->getSampleSets().end();
				_sceneNode--;
				setSample();
			}
			else if(Keyboard::get().isStartPressing(Key::D))
			{
				_sample++;
				if(_sample == _sceneNode->second->getSamples().end())
					_sample = _sceneNode->second->getSamples().begin();
				setStartTime();
			}
			else if(Keyboard::get().isStartPressing(Key::A))
			{
				if(_sample == _sceneNode->second->getSamples().begin())
					_sample = _sceneNode->second->getSamples().end();
				_sample--;
				setStartTime();
			}
			std::stringstream s;
			s << _sceneNode->first.getString() << " " << _startTime;
			Graphics::get().setTitle(s.str().c_str());
		}

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTITY_INIT)
			{
				_animationSet = ResourceManager::get().getAnimationSet("resources/animations/skeleton.xml");
				_animation = _animationSet->get().begin();
				_sceneNode = _animation->second->getSampleSets().begin();
				_sample = _sceneNode->second->getSamples().begin();
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				update();
			}
		}

		Component* clone() const { return 0; }
	private:
		Vector _offset;
		bool _translation;
		float _startTime;

		std::shared_ptr<AnimationSet> _animationSet;
		AnimationIterator _animation;
		SampleSetIterator _sceneNode;
		SampleIterator _sample;
		float _time;
	};

	class MyGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState)
		{
			if(id == Hash("resources/game-states/test.xml"))
			{
				Entity* entity = new Entity();
				entity->add(new AnimationEditor());
				gameState.getEntitiesManager().add(Hash("ENT_ANIMATION_EDITOR"), entity);
			}
		}
	};
}