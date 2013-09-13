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
		AnimationEditor() : _offset(Vector::Zero), _translation(true) {}

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
				const Vector& position = Mouse::get().getPosition();
				Sample& sample = (**_sample);
				static int i = 0;
				if(Mouse::get().isStartClicking(MouseButton::LEFT))
				{
					_offset = Mouse::get().getPosition() - sample.getTranslation();
					_translation = true;
				}
				else if(Mouse::get().isClicking(MouseButton::LEFT))
				{
					sample.setTranslation(position - _offset);
					_animation->second->init();
				}
				else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
				{
					_offset = Mouse::get().getPosition() - sample.getTranslation();
					_translation = false;
				}
				else if(Mouse::get().isClicking(MouseButton::RIGHT))
				{
					const Vector vector = Mouse::get().getPosition() - _offset;
					float rotation = fromRadians(vector.getAngle());
					sample.setRotation(rotation);
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
		}

		Component* clone() const { return 0; }
	private:
		Vector _offset;
		bool _translation;

		std::shared_ptr<AnimationSet> _animationSet;
		AnimationIterator _animation;
		SampleSetIterator _sceneNode;
		SampleIterator _sample;
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