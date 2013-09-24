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
		AnimationEditor() : _offset(Vector::Zero), _translation(true), _frame(0), _copyFrame(0) {}

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
			_frame = 0;
			setSample();
		}

		float getStartTime()
		{
			return _frame * FRAME_TIME;
		}

		void setSample()
		{
			float startTime = getStartTime();
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &startTime));
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

		SampleIterator addSample(SampleIterator sampleIterator, float duration2)
		{
			const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
			const SceneNode& node = *root.get(_sceneNode->second->getId());
			Sample* sample = new Sample();
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			sample->setDuration(duration2);
			float duration1 = getStartTime() - (**sampleIterator).getStartTime();
			(**sampleIterator).setDuration(duration1);
			sampleIterator = _sceneNode->second->getSamples().insert(sampleIterator+1, sample);
			_animation->second->init();
			return sampleIterator;
		}

		SampleIterator getSampleIterator()
		{
			SampleCollection& samples = _sceneNode->second->getSamples();
			SampleIterator sampleIterator;
			float startTime = getStartTime();
			for(sampleIterator = samples.begin(); sampleIterator != samples.end() && (**sampleIterator).getStartTime() < startTime; ++sampleIterator);
			return sampleIterator;
		}

		Sample& getSample()
		{
			float startTime = getStartTime();
			SampleIterator sampleIterator = getSampleIterator();
			// Frame not exist yet
			if(sampleIterator == _sceneNode->second->getSamples().end())
			{
				--sampleIterator;
				sampleIterator = addSample(sampleIterator, FRAME_TIME);
			}
			// Split Frame
			else if((**sampleIterator).getStartTime() != startTime)
			{
				--sampleIterator;
				float duration2 = (**sampleIterator).getDuration() - startTime + (**sampleIterator).getStartTime();
				sampleIterator = addSample(sampleIterator, duration2);
			}
			return **sampleIterator;
		}

		void pasteFrame()
		{
			Sample& pasteSample = getSample();
			int tempFrame = _frame;
			_frame = _copyFrame;
			Sample& copySample = getSample();
			_frame = tempFrame;
			pasteSample.setTranslation(copySample.getTranslation());
			pasteSample.setRotation(copySample.getRotation());
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
				float startTime = getStartTime();
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &startTime));
			}
			else if(Keyboard::get().isStartPressing(Key::C))
			{
				_copyFrame = _frame;
			}
			else if(Keyboard::get().isStartPressing(Key::V))
			{
				SampleSetIterator tempSceneNode = _sceneNode;
				for(SampleSetIterator i = _animation->second->getSampleSets().begin(); i != _animation->second->getSampleSets().end(); ++i)
				{
					_sceneNode = i;
					pasteFrame();
				}
				_sceneNode = tempSceneNode;
			}
			else if(Keyboard::get().isStartPressing(Key::DELETE))
			{
				SampleIterator i = getSampleIterator();
				if(i == _sceneNode->second->getSamples().begin() ||
				   i == _sceneNode->second->getSamples().end() ||
				   (**i).getStartTime() != getStartTime())
					return;

				Sample* sample = *i;
				i = _sceneNode->second->getSamples().erase(i);
				--i;
				(**i).setDuration((**i).getDuration() + sample->getDuration());
				delete sample;

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
				++_frame;
				setSample();
			}
			else if(Keyboard::get().isStartPressing(Key::A))
			{
				if(_frame > 0)
					--_frame;
				setSample();
			}
			else if(Keyboard::get().isStartPressing(Key::F2))
			{
				XmlSerializer serializer(new FileStream("C:/Users/Rolllo1U/Documents/Visual Studio 2012/Projects/Temporal/Temporal/Temporal/External/bin/resources/animations/aquaria.xml", true, false));
				AnimationSet& set =  *_animationSet;
				serializer.serialize("animation-set", set);
				serializer.save();

			}
			std::stringstream s;
			s << _animation->first.getString() << " " << _sceneNode->first.getString() << " " << _frame;
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
		static const float FRAME_TIME;

		Vector _offset;
		bool _translation;

		std::shared_ptr<AnimationSet> _animationSet;
		AnimationIterator _animation;
		SampleSetIterator _sceneNode;
		int _frame;

		int _copyFrame;
	};

	const float AnimationEditor::FRAME_TIME = 0.067f;

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