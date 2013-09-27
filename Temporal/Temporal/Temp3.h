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
#include "Layer.h"
#include <algorithm>
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
			Sample& sample = getOrCreateSample();
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

		void setAnimation()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &_animation));
			_frame = 0;
			setSample();
		}

		float getStartTime(int frame = -1)
		{
			if(frame == -1)
				frame = _frame;
			return frame * FRAME_TIME;
		}

		void setSample()
		{
			float startTime = getStartTime();
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &startTime));
		}

		SampleIterator addSample(SampleIterator sampleIterator, float duration2)
		{
			const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
			const SceneNode& node = *root.get(_sceneNode);
			Sample* sample = new Sample();
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			sample->setDuration(duration2);
			float duration1 = getStartTime() - (**sampleIterator).getStartTime();
			(**sampleIterator).setDuration(duration1);
			Animation& animation = *_animationSet->get().at(_animation);
			sampleIterator = animation.getSampleSets().at(_sceneNode)->getSamples().insert(sampleIterator+1, sample);
			animation.init();
			return sampleIterator;
		}

		SampleCollection* getSceneNode(Hash animation = Hash::INVALID, Hash sceneNode = Hash::INVALID)
		{
			if(animation == Hash::INVALID)
				animation = _animation;
			if(sceneNode == Hash::INVALID)
				sceneNode = _sceneNode;
			AnimationIterator i = _animationSet->get().find(animation);
			if(i == _animationSet->get().end())
				return 0;
			SampleSetIterator j = i->second->getSampleSets().find(sceneNode);
			if(j == i->second->getSampleSets().end())
				return 0;
			SampleCollection& samples = j->second->getSamples();
			return &samples;
		}

		SampleIterator getSampleIterator(SampleCollection& samples, int frame = -1)
		{
			SampleIterator sampleIterator;
			float startTime = getStartTime(frame);
			for(sampleIterator = samples.begin(); sampleIterator != samples.end() && (**sampleIterator).getStartTime() < startTime; ++sampleIterator);
			return sampleIterator;
		}

		Sample* getSample(Hash animation = Hash::INVALID, Hash sceneNode = Hash::INVALID, int frame = -1)
		{
			SampleCollection* samples = getSceneNode(animation, sceneNode);
			if(!samples)
				return 0;
			SampleIterator i = getSampleIterator(*samples, frame);
			if(i == samples->end())
				return 0;
			return *i;
			return 0;
		}

		Sample& getOrCreateSample()
		{
			SampleCollection* samples = getSceneNode();
			// TODO: create sample...
			SampleIterator sampleIterator = getSampleIterator(*samples);
			float startTime = getStartTime();
			// Frame not exist yet
			if(sampleIterator == samples->end())
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
			Hash tempSceneNode = _sceneNode;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				_sceneNode = *i;
				Sample& pasteSample = getOrCreateSample();
				int tempFrame = _frame;
				_frame = _copyFrame;
				Sample& copySample = getOrCreateSample();
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
			SampleCollection* samples = getSceneNode();
			if(!samples)
				return;
			addUndo();
			SampleIterator i = getSampleIterator(*samples);
			if(i == samples->begin() ||
				i == samples->end() ||
				(**i).getStartTime() != getStartTime())
				return;

			Sample* sample = *i;
			i = samples->erase(i);
			--i;
			(**i).setDuration((**i).getDuration() + sample->getDuration());
			delete sample;
		}

		void update()
		{
			if(Mouse::get().isStartClicking(MouseButton::LEFT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getOrCreateSample().getTranslation();
				_translation = true;
			}
			else if(Mouse::get().isClicking(MouseButton::LEFT))
			{
				getOrCreateSample().setTranslation(Mouse::get().getPosition() - _offset);
				_animationSet->get().at(_animation)->init();
			}
			else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getOrCreateSample().getTranslation();
				_translation = false;
			}
			else if(Mouse::get().isClicking(MouseButton::RIGHT))
			{
				const Vector vector = Mouse::get().getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getOrCreateSample().setRotation(rotation);
				_animationSet->get().at(_animation)->init();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_UP))
			{
				AnimationIterator i = _animationSet->get().find(_animation);
				++i;
				if(i != _animationSet->get().end())
					_animation = i->first;
				setAnimation();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_DOWN))
			{
				AnimationIterator i = _animationSet->get().find(_animation);
				if(i != _animationSet->get().begin())
				{
					--i;
					_animation = i->first;
				}
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
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNode);
				++i;
				if(i != _sceneNodes.end())
					_sceneNode = *i;
			}
			else if(Keyboard::get().isStartPressing(Key::S))
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNode);
				if(i != _sceneNodes.begin())
				{
					--i;
					_animation = *i;
				}
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
			s << _animation.getString() << " " << _sceneNode.getString() << " " << _frame;
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
			_animation = _animationSet->get().begin()->first;
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNode = *_sceneNodes.begin();
			setAnimation();

			float y = GRID_START_Y;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				y -= CELL_SIZE;
				addLabel(i->getString(), Vector(0.0f, y), GRID_START_X);
			}
			for(int i = 0;  i < GRID_FRAMES; ++i)
			{
				addLabel(Utils::toString(i+1).c_str(), Vector(GRID_START_X + i * CELL_SIZE, GRID_START_Y), CELL_SIZE);
			}
		}

		void draw()
		{
			float y = GRID_START_Y;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				for(int j = 0;  j < GRID_FRAMES; ++j)
				{
					//Sample* sample = getSample(_animation, *i, j);
					//bool fill = sample && sample->getStartTime() == getStartTime(j);
					Graphics::get().draw(AABBLT(GRID_START_X + j * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					
				}
				y -= CELL_SIZE;
			}

			Animation& animation = *_animationSet->get().find(_animation)->second;
			for(SampleSetIterator i = animation.getSampleSets().begin(); i != animation.getSampleSets().end(); ++i)
			{
				int j = 0;
				for(HashIterator k = _sceneNodes.begin(); k != _sceneNodes.end(); ++k)	
				{
					if(i->first == *k)
						break;
					++j;
				}
				for(SampleIterator l = i->second->getSamples().begin(); l != i->second->getSamples().end(); ++l)
				{
					float m = (**l).getStartTime() / FRAME_TIME;
					Graphics::get().draw(AABBLT(GRID_START_X + m * CELL_SIZE, GRID_START_Y - j * CELL_SIZE, CELL_SIZE, CELL_SIZE), Color::White, true);
				}
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
				LayerType::Enum layer = *static_cast<LayerType::Enum*>(message.getParam());
				if(layer == LayerType::GUI)
					draw();
			}
		}

		Component* clone() const { return 0; }
	private:
		static const float FRAME_TIME;
		static const float CELL_SIZE;
		static const float GRID_START_X;
		static const float GRID_START_Y;
		static const int GRID_FRAMES;

		std::shared_ptr<AnimationSet> _animationSet;
		Hash _animation;
		HashCollection _sceneNodes;
		Hash _sceneNode;
		int _frame;

		Vector _offset;
		bool _translation;
		AnimationSet* _undo;
		int _copyFrame;
	};

	const float AnimationEditor::FRAME_TIME = 0.067f;
	const float AnimationEditor::CELL_SIZE = 16.0f;
	const float AnimationEditor::GRID_START_X = 128.0f;
	const float AnimationEditor::GRID_START_Y = 256.0f;
	const int AnimationEditor::GRID_FRAMES = 48;

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