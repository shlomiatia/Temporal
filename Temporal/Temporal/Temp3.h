#include "Hash.h"
#include "GameState.h"
#include "EntitySystem.h"
#include "Animation.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Math.h"
#include "SceneNode.h"
#include "Serialization.h"
#include "SerializationAccess.h"

#include <sstream>
#include "Graphics.h"

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero), _translation(true), _startTime(0.0f), _duration(0.0f) {}

		Hash getType() const { return Hash::INVALID; }

		void handleArrows(const Vector& vector)
		{
			if(_translation)
			{
				getSample().setTranslation(getSample().getTranslation() + vector);
			}
			else
			{
				float angle = getSample().getRotation() + vector.getX() + vector.getY();
				if(abs(angle) > 180.0f)
				{
					angle = (angle > 0.0f ? -360.0f : 360.0f) + angle;
				}
				getSample().setRotation(angle);
			}
		}

		void setAnimation()
		{
			Hash id = _animation->first;
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &id));
			_sceneNode = _animation->second->getSampleSets().begin();
			setSample(_sceneNode->second->getSamples().begin());
		}

		void setSample(SampleIterator i)
		{
			_startTime = (**i).getStartTime();
			_duration = (**i).getDuration();
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_startTime));
		}

		template<class T>
		typename T::const_iterator cyclicIncrease(typename T::const_iterator i, T& v)
		{
			i++;
			if(i == v.end())
				i = v.begin();
			return i;
		}

		template<class T>
		typename T::const_iterator cyclicDecrease(typename T::const_iterator i, T& v)
		{
			if(i == v.begin())
				i = v.end();
			i--;
			return i;
		}

		SampleIterator getSampleIterator()
		{
			SampleCollection& samples = _sceneNode->second->getSamples();
			SampleIterator sampleIterator;
			for(sampleIterator = samples.begin(); sampleIterator != samples.end() && (**sampleIterator).getStartTime() < _startTime; ++sampleIterator);
			if(sampleIterator == samples.end())
			{
				--sampleIterator;
				const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
				const SceneNode& node = *root.get(_sceneNode->second->getId());
				Sample* sample = new Sample();
				sample->setTranslation(node.getTranslation());
				sample->setRotation(node.getRotation());
				sample->setDuration(_duration);
				(**sampleIterator).setDuration(_startTime - (**sampleIterator).getStartTime());
				sampleIterator = _sceneNode->second->getSamples().insert(sampleIterator+1, sample);
				_animation->second->init();
			}
			return sampleIterator;
		}

		Sample& getSample()
		{
			return **getSampleIterator();
		}

		void update()
		{
			if(Mouse::get().isStartClicking(MouseButton::LEFT))
			{
				_offset = Mouse::get().getPosition() - getSample().getTranslation();
				_translation = true;
			}
			else if(Mouse::get().isClicking(MouseButton::LEFT))
			{
				getSample().setTranslation(Mouse::get().getPosition() - _offset);
				_animation->second->init();
			}
			else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
			{
				_offset = Mouse::get().getPosition() - getSample().getTranslation();
				_translation = false;
			}
			else if(Mouse::get().isClicking(MouseButton::RIGHT))
			{
				const Vector vector = Mouse::get().getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getSample().setRotation(rotation);
				_animation->second->init();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_UP))
			{
				_animation = cyclicIncrease(_animation, _animationSet->get());
				setAnimation();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_DOWN))
			{
				_animation = cyclicDecrease(_animation, _animationSet->get());
				setAnimation();
			}
			else if(Keyboard::get().isPressing(Key::PLUS))
			{
				if(getSample().getStartTime() == _startTime)
				{
					getSample().setDuration(getSample().getDuration() + 0.01f);
					_duration = getSample().getDuration();
					_animation->second->init();
				}
			}
			else if(Keyboard::get().isPressing(Key::MINUS))
			{
				if(getSample().getStartTime() == _startTime)
				{
					float duration = getSample().getDuration() - 0.01;
					if(duration < 0.0f)
						duration = 0.0f;
					getSample().setDuration(duration);;
					_duration = getSample().getDuration();
					_animation->second->init();
				}
			}
			else if(Keyboard::get().isStartPressing(Key::UP))
			{
				handleArrows(Vector(0.0f, 1.0f));
			}
			else if(Keyboard::get().isStartPressing(Key::DOWN))
			{
				handleArrows(Vector(0.0f, -1.0f));
			}
			else if(Keyboard::get().isStartPressing(Key::LEFT))
			{
				handleArrows(Vector(-1.0f, 0.0f));
			}
			else if(Keyboard::get().isStartPressing(Key::RIGHT))
			{
				handleArrows(Vector(1.0f, 0.0f));
			}
			else if(Keyboard::get().isStartPressing(Key::SPACE))
			{
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::TOGGLE_ANIMATION));
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_startTime));
			}
			else if(Keyboard::get().isStartPressing(Key::C))
			{
				SampleIterator i = getSampleIterator();
				Sample* sample = new Sample();
				sample->setTranslation((**i).getTranslation());
				sample->setRotation((**i).getRotation());
				sample->setDuration((**i).getDuration());
				i = _sceneNode->second->getSamples().insert(i+1, sample);
				_animation->second->init();
				setSample(i);
			}
			else if(Keyboard::get().isStartPressing(Key::W))
			{
				_sceneNode = cyclicIncrease(_sceneNode, _animation->second->getSampleSets());

			}
			else if(Keyboard::get().isStartPressing(Key::S))
			{
				_sceneNode = cyclicDecrease(_sceneNode, _animation->second->getSampleSets());
			}
			else if(Keyboard::get().isStartPressing(Key::D))
			{
				SampleIterator i = cyclicIncrease(getSampleIterator(), _sceneNode->second->getSamples());
				setSample(i);
			}
			else if(Keyboard::get().isStartPressing(Key::A))
			{
				SampleIterator i = cyclicDecrease(getSampleIterator(), _sceneNode->second->getSamples());
				setSample(i);
			}
			else if(Keyboard::get().isStartPressing(Key::F2))
			{
				XmlSerializer serializer(new FileStream("C:/Users/SHLOMIATIA/Documents/Visual Studio 2010/Projects/Temporal/Temporal/Temporal/External/bin/resources/animations/aquaria.xml", true, false));
				AnimationSet& set =  *_animationSet;
				serializer.serialize("animation-set", set);
				serializer.save();

			}
			std::stringstream s;
			s << _animation->first.getString() << " " << _sceneNode->first.getString() << " " << _startTime << " " << _duration;
			Graphics::get().setTitle(s.str().c_str());
		}

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTITY_INIT)
			{
				_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
				_animation = _animationSet->get().begin();
				setAnimation();
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
		float _duration;

		std::shared_ptr<AnimationSet> _animationSet;
		AnimationIterator _animation;
		SampleSetIterator _sceneNode;
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