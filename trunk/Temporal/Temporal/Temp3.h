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
		AnimationEditor() : _offset(Vector::Zero), _translation(true), _index(0), _copyIndex(0), _undo(0) {}

		Hash getType() const { return Hash::INVALID; }

		void setAnimation()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &_animationId));
			_index = 0;
			setSample();
		}

		void setSample()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_index));
		}

		SceneGraphSampleIterator getSceneGraphSampleIterator(SceneGraphSampleCollection*& samples, int index = -1, bool createSceneGraphSample = false)
		{
			if(index == -1)
				index = _index;
			samples = &_animationSet->get(_animationId).getSamples();
			for(SceneGraphSampleIterator i = samples->begin();; ++i)
			{
				if(createSceneGraphSample && (i == samples->end() || (**i).getIndex() >= index))
				{
					return samples->insert(i, new SceneGraphSample(index));
				}
				else if(i == samples->end() || (**i).getIndex() == index)
				{
					return i;
				}
			}
		}

		SceneGraphSample& getSceneGraphSample(int index = -1)
		{
			if(index == -1)
				index = _index;
			SceneGraphSampleCollection* samples = 0;
			return **getSceneGraphSampleIterator(samples, index, true);
		}

		SceneNodeSampleIterator getSceneNodeSampleIterator(SceneNodeSampleCollection* samples, int index = -1, Hash sceneNodeId = Hash::INVALID, bool createSceneGraphSample = false)
		{
			if(sceneNodeId == Hash::INVALID)
				sceneNodeId = _sceneNodeId;
			SceneGraphSampleCollection* sceneGraphSamples = 0;
			SceneGraphSampleIterator i = getSceneGraphSampleIterator(sceneGraphSamples, index, createSceneGraphSample);
			SceneNodeSampleIterator j;
			if(i == sceneGraphSamples->end())
				return j;
			samples = &(**i).getSamples();
			j = samples->find(sceneNodeId);
			return j;
		}

		SceneNodeSample& addSample(SceneNodeSampleCollection& samples, SceneNodeSampleIterator iterator)
		{
			const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
			const SceneNode& node = *root.get(_sceneNodeId);
			SceneNodeSample* sample = new SceneNodeSample();
			sample->setTranslation(node.getTranslation());
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			_animationSet->get(_animationId).init();
			samples[sample->getId()] = sample;
			return *sample;
		}

		SceneNodeSample& getSceneNodeSample(int index = -1, Hash sceneNodeId = Hash::INVALID)
		{
			SceneNodeSampleCollection* samples = 0;
			SceneNodeSampleIterator i = getSceneNodeSampleIterator(samples, index, sceneNodeId, true);
			if(i == samples->end())
				return addSample(*samples, i);
			return *i->second;
		}

		void paste()
		{
			addUndo();
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				SceneNodeSampleCollection* sceneNodeSamples = 0;
				SceneNodeSampleIterator j = getSceneNodeSampleIterator(sceneNodeSamples, _copyIndex, *i);
				if(!sceneNodeSamples)
					continue;
				SceneNodeSample& copy = *j->second;
				SceneNodeSample& paste = getSceneNodeSample(_index, *i);
				paste.setTranslation(copy.getTranslation());
				paste.setRotation(copy.getRotation());
			}
		}

		void addUndo()
		{
			if(_undo)
				delete _undo;
			_undo = _animationSet->clone();
		}

		void deleteSample()
		{
			SceneNodeSampleCollection* sceneNodeSamples = 0;
			SceneNodeSampleIterator i = getSceneNodeSampleIterator(sceneNodeSamples);
			if(!sceneNodeSamples || i == sceneNodeSamples->end() || i->second->getParent().getIndex() == 0)
				return;
			addUndo();
			SceneNodeSample* sample = i->second;
			i = sceneNodeSamples->erase(i);
			delete sample;
			if(sceneNodeSamples->size() == 0)
			{
				SceneGraphSampleCollection* sceneGraphSamples = 0;
				SceneGraphSampleIterator j = getSceneGraphSampleIterator(sceneGraphSamples);
				j = sceneGraphSamples->erase(j);
			}
		}

		void handleArrows(const Vector& vector)
		{
			SceneNodeSample& sample = getSceneNodeSample();
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

		void update()
		{
			if(Mouse::get().isStartClicking(MouseButton::LEFT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getSceneNodeSample().getTranslation();
				_translation = true;
			}
			else if(Mouse::get().isClicking(MouseButton::LEFT))
			{
				getSceneNodeSample().setTranslation(Mouse::get().getPosition() - _offset);
				_animationSet->getAnimations().at(_animationId)->init();
			}
			else if(Mouse::get().isStartClicking(MouseButton::RIGHT))
			{
				addUndo();
				_offset = Mouse::get().getPosition() - getSceneNodeSample().getTranslation();
				_translation = false;
			}
			else if(Mouse::get().isClicking(MouseButton::RIGHT))
			{
				const Vector vector = Mouse::get().getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getSceneNodeSample().setRotation(rotation);
				_animationSet->getAnimations().at(_animationId)->init();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_UP))
			{
				AnimationIterator i = _animationSet->getAnimations().find(_animationId);
				++i;
				if(i != _animationSet->getAnimations().end())
					_animationId = i->first;
				setAnimation();
			}
			else if(Keyboard::get().isStartPressing(Key::PAGE_DOWN))
			{
				AnimationIterator i = _animationSet->getAnimations().find(_animationId);
				if(i != _animationSet->getAnimations().begin())
				{
					--i;
					_animationId = i->first;
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
				_copyIndex = _index;
			}
			else if(Keyboard::get().isStartPressing(Key::V))
			{
				paste();
			}
			else if(Keyboard::get().isStartPressing(Key::DELETE))
			{
				deleteSample();
			}
			else if(Keyboard::get().isStartPressing(Key::W))
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				++i;
				if(i != _sceneNodes.end())
					_sceneNodeId = *i;
			}
			else if(Keyboard::get().isStartPressing(Key::S))
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				if(i != _sceneNodes.begin())
				{
					--i;
					_animationId = *i;
				}
			}
			else if(Keyboard::get().isStartPressing(Key::D))
			{
				++_index;
				setSample();
			}
			else if(Keyboard::get().isStartPressing(Key::A))
			{
				if(_index > 0)
					--_index;
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
			s << _animationId.getString() << " " << _sceneNodeId.getString() << " " << _index;
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
			_animationId = _animationSet->getAnimations().begin()->first;
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNodeId = *_sceneNodes.begin();
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
					Graphics::get().draw(AABBLT(GRID_START_X + j * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					
				}
				y -= CELL_SIZE;
			}

			Animation& animation = _animationSet->get(_animationId);
			for(SceneGraphSampleIterator i = animation.getSamples().begin(); i != animation.getSamples().end(); ++i)
			{
				SceneGraphSample& sceneGraphSample = **i;
				int j = 0;
				for(HashIterator k = _sceneNodes.begin(); k != _sceneNodes.end(); ++k)	
				{
					 SceneNodeSampleIterator l = sceneGraphSample.getSamples().find(*k);
					 if(l != sceneGraphSample.getSamples().end())
					 {
						 Graphics::get().draw(AABBLT(GRID_START_X + sceneGraphSample.getIndex() * CELL_SIZE, GRID_START_Y - j * CELL_SIZE, CELL_SIZE, CELL_SIZE), Color::White, true);
					 }
					++j;
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
		Hash _animationId;
		HashCollection _sceneNodes;
		Hash _sceneNodeId;
		int _index;

		Vector _offset;
		bool _translation;
		AnimationSet* _undo;
		int _copyIndex;
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