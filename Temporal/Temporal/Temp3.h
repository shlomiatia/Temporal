#include "Hash.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Animation.h"
#include "Mouse.h"
#include "Graphics.h"
#include <sstream>

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero) {}

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
				const Vector& position = Mouse::get().getPosition();
				Sample& sample = (**_sample);
				if(Mouse::get().isStartClicking(MouseButton::LEFT))
				{
					_offset = Mouse::get().getPosition() - sample.getTranslation();
				}
				else if(Mouse::get().isClicking(MouseButton::LEFT))
				{
					sample.setTranslation(position - _offset);
					_animation->second->init();
				}
				std::ostringstream s;
				s << "position:" << position.getX() << "," << position.getY() <<
					 "offset:" << _offset.getX() << "," << _offset.getY() <<
					 "translation:" << sample.getTranslation().getX() << "," << sample.getTranslation().getY();
				Graphics::get().setTitle(s.str().c_str());
				
			}
		}

		Component* clone() const { return 0; }
	private:
		Vector _offset;
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