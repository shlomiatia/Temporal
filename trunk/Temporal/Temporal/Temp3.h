#include "Hash.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Animation.h"
#include "Mouse.h"

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _mouseLastPosition(Vector::Zero) {}

		Hash getType() const { return Hash::INVALID; }

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
				if(Mouse::get().isStartClicking(MouseButton::LEFT))
				{
					_mouseLastPosition = Mouse::get().getPosition();
				}
				else if(Mouse::get().isClicking(MouseButton::LEFT))
				{
					const Vector& position = Mouse::get().getPosition();
					Sample& sample = (**_sample);
					sample.setTranslation(sample.getTranslation() + position - _mouseLastPosition);
					_mouseLastPosition = position;
					_animation->second->init();
				}
			}
		}

		Component* clone() const { return 0; }
	private:
		Vector _mouseLastPosition;
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
			if(id == Hash("resources/game-states/entities.xml"))
			{
				Entity* entity = new Entity();
				entity->add(new AnimationEditor());
				gameState.getEntitiesManager().add(Hash("ENT_ANIMATION_EDITOR"), entity);
			}
		}
	};
}