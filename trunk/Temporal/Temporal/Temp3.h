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
#include "Text.h"
#include "Transform.h"
#include "Utils.h"
#include "Graphics.h"
#include "Shapes.h"

#include <sstream>


// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero), _translation(true), _frame(0), _copyFrame(0), _undo(0) {}

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
			Hash id = *_animation;
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &id));
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
			const SceneNode& node = *root.get(*_sceneNode);
			Sample* sample = new Sample();
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			sample->setDuration(duration2);
			float duration1 = getStartTime() - (**sampleIterator).getStartTime();
			(**sampleIterator).setDuration(duration1);
			Animation& animation = *_animationSet->get().at(*_animation);
			sampleIterator = animation.getSampleSets().at(*_sceneNode)->getSamples().insert(sampleIterator+1, sample);
			animation.init();
			return sampleIterator;
		}

		SampleIterator getSampleIterator()
		{
			SampleCollection& samples = _animationSet->get().at(*_animation)->getSampleSets().at(*_sceneNode)->getSamples();
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
			if(sampleIterator == _animationSet->get().at(*_animation)->getSampleSets().at(*_sceneNode)->getSamples().end())
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

		void paste()
		{
			addUndo();
			HashIterator tempSceneNode = _sceneNode;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				_sceneNode = i;
				Sample& pasteSample = getSample();
				int tempFrame = _frame;
				_frame = _copyFrame;
				Sample& copySample = getSample();
				_frame = tempFrame;
				pasteSample.setTranslation(copySample.getTranslation());
				pasteSample.setRotation(copySample.getRotation());
			}
			_sceneNode = tempSceneNode;
		}

		void addUndo()
		{
			if(_undo)
				delete _undo;
			_undo = _animationSet->clone();
		}

		void deleteFrame()
		{
			addUndo();
			SampleIterator i = getSampleIterator();
			if(i == _animationSet->get().at(*_animation)->getSampleSets().at(*_sceneNode)->getSamples().begin() ||
				i == _animationSet->get().at(*_animation)->getSampleSets().at(*_sceneNode)->getSamples().end() ||
				(**i).getStartTime() != getStartTime())
				return;

			Sample* sample = *i;
			i = _animationSet->get().at(*_animation)->getSampleSets().at(*_sceneNode)->getSamples().erase(i);
			--i;
			(**i).setDuration((**i).getDuration() + sample->getDuration());
			delete sample;
		}

		void update()
		{
			if(Mouse::get().isStartClicking(MouseButton::LEFT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getSample().getTranslation();
				_translation = true;
			}
			else if(Mouse::get().isClicking(MouseButton::LEFT))
			{
				getSample().setTranslation(Mouse::get().getPosition() - _offset);
				_animationSet->get().at(*_animation)->init();
			}
			else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getSample().getTranslation();
				_translation = false;
			}
			else if(Mouse::get().isClicking(MouseButton::RIGHT))
			{
				const Vector vector = Mouse::get().getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getSample().setRotation(rotation);
				_animationSet->get().at(*_animation)->init();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_UP))
			{
				_animation = cyclicIncrease(_animation, _animations);
				setAnimation();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_DOWN))
			{
				_animation = cyclicDecrease(_animation, _animations);
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
				setSample();
			}
			else if(Keyboard::get().isStartPressing(Key::Z))
			{
				if(_undo)
				{
					std::shared_ptr<AnimationSet> undo(_undo);
					ResourceManager::get()._animationSets[Hash("resources/animations/aquaria.xml")] = undo;
					undo->init();
					_animationSet = undo;
					getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::ENTITY_INIT));
					_undo = 0;
				}
			}
			else if(Keyboard::get().isStartPressing(Key::C))
			{
				_copyFrame = _frame;
			}
			else if(Keyboard::get().isStartPressing(Key::V))
			{
				paste();
			}
			else if(Keyboard::get().isStartPressing(Key::DELETE))
			{
				deleteFrame();

			}
			else if(Keyboard::get().isStartPressing(Key::W))
			{
				_sceneNode = cyclicIncrease(_sceneNode, _sceneNodes);
			}
			else if(Keyboard::get().isStartPressing(Key::S))
			{
				_sceneNode = cyclicDecrease(_sceneNode, _sceneNodes);
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
				XmlSerializer serializer(new FileStream("C:/Users/SHLOMIATIA/Documents/Visual Studio 2010/Projects/Temporal/Temporal/Temporal/External/bin/resources/animations/aquaria.xml", true, false));
				AnimationSet& set =  *_animationSet;
				serializer.serialize("animation-set", set);
				serializer.save();

			}
			std::stringstream s;
			s << _animation->getString() << " " << _sceneNode->getString() << " " << _frame;
			Graphics::get().setTitle(s.str().c_str());
		}

		void addLabel(const char* label, const Vector& position, float width)
		{
			Transform* transform = new Transform(position);
			Text* text = new Text("c:/windows/fonts/Arial.ttf", 12, label);
			text->setWidth(width);
			Entity* entity = new Entity();
			entity->add(transform);
			entity->add(text);
			entity->handleMessage(Message(MessageID::ENTITY_INIT));
			getEntity().getManager().add(Hash(label), entity);
		}

		void bindSceneNodes(SceneNode& sceneNode)
		{
			_sceneNodes.push_back(sceneNode.getID());
			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				bindSceneNodes(**i);
			}
		}

		void init()
		{
			_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
			for(AnimationIterator i = _animationSet->get().begin(); i != _animationSet->get().end(); ++i)
			{
				_animations.push_back(i->first);
			}
			_animation = _animations.begin();
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNode = _sceneNodes.begin();
			setAnimation();

			
			float y = GRID_START_Y;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				y -= CELL_SIZE;
				addLabel(i->getString(), Vector(0.0f, y), GRID_START_X);
			}
			for(int i = 0;  i < 24; ++i)
			{
				addLabel(Utils::toString(i+1).c_str(), Vector(GRID_START_X + i * CELL_SIZE, GRID_START_Y), CELL_SIZE);
			}
		}

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTITY_INIT)
			{
				init();
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				update();
			}
			else if(message.getID() == MessageID::DRAW)
			{
				float y = GRID_START_Y;
				for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
				{
					for(int i = 0;  i < 24; ++i)
					{
						Graphics::get().draw(AABBLT(GRID_START_X + i * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					}
					y -= CELL_SIZE;
				}
			}
		}

		Component* clone() const { return 0; }
	private:
		static const float FRAME_TIME;
		static const float CELL_SIZE;
		static const float GRID_START_X;
		static const float GRID_START_Y;

		std::shared_ptr<AnimationSet> _animationSet;
		HashCollection _animations;
		HashIterator _animation;
		HashCollection _sceneNodes;
		HashIterator _sceneNode;
		int _frame;

		Vector _offset;
		bool _translation;
		AnimationSet* _undo;
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