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
#include "MessageUtils.h"
#include <algorithm>
#include <sstream>

#include "Delegate.h"

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero), _translation(false), _rotation(false), _index(0), _copyIndex(0), _undo(0) {}

		Hash getType() const { return Hash::INVALID; }

		void setAnimation(int index = 0)
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &_animationId));
			_index = index;
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
				if(createSceneGraphSample && (i == samples->end() || (**i).getIndex() > index))
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

		SceneNodeSampleIterator getSceneNodeSampleIterator(SceneNodeSampleCollection*& samples, int index = -1, Hash sceneNodeId = Hash::INVALID, bool createSceneGraphSample = false)
		{
			if(sceneNodeId == Hash::INVALID)
				sceneNodeId = _sceneNodeId;
			SceneGraphSampleCollection* sceneGraphSamples = 0;
			SceneGraphSampleIterator i = getSceneGraphSampleIterator(sceneGraphSamples, index, createSceneGraphSample);
			samples = &(**i).getSamples();
			SceneNodeSampleIterator j = samples->find(sceneNodeId);
			return j;
		}

		SceneNodeSample& addSample(SceneNodeSampleCollection& samples, SceneNodeSampleIterator iterator)
		{
			const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
			const SceneNode& node = *root.get(_sceneNodeId);
			SceneNodeSample* sample = new SceneNodeSample(_sceneNodeId);
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			samples[sample->getId()] = sample;
			_animationSet->get(_animationId).init();
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

		void handleKey(Key::Enum key)
		{
			if(key == Key::PAGE_UP)
			{
				AnimationIterator i = _animationSet->getAnimations().find(_animationId);
				++i;
				if(i != _animationSet->getAnimations().end())
					_animationId = i->first;
				setAnimation();
			}
			else if(key == Key::PAGE_DOWN)
			{
				AnimationIterator i = _animationSet->getAnimations().find(_animationId);
				if(i != _animationSet->getAnimations().begin())
				{
					--i;
					_animationId = i->first;
				}
				setAnimation();
			}
			else if(key == Key::UP)
			{
				handleArrows(Vector(0.0f, 1.0f));
			}
			else if(key == Key::DOWN)
			{
				handleArrows(Vector(0.0f, -1.0f));
			}
			else if(key == Key::LEFT)
			{
				handleArrows(Vector(-1.0f, 0.0f));
			}
			else if(key == Key::RIGHT)
			{
				handleArrows(Vector(1.0f, 0.0f));
			}
			else if(key == Key::SPACE)
			{
				getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::TOGGLE_ANIMATION));
				setSample();
			}
			else if(key == Key::Z)
			{
				if(_undo)
				{
					std::shared_ptr<AnimationSet> undo(_undo);
					ResourceManager::get()._animationSets[Hash("resources/animations/aquaria.xml")] = undo;
					undo->init();
					_animationSet = undo;
					getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::ENTITY_INIT));
					setAnimation(_index);
					_undo = 0;
				}
			}
			else if(key == Key::C)
			{
				_copyIndex = _index;
			}
			else if(key == Key::V)
			{
				paste();
			}
			else if(key == Key::DELETE)
			{
				deleteSample();
			}
			else if(key == Key::W)
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				++i;
				if(i != _sceneNodes.end())
					_sceneNodeId = *i;
			}
			else if(key == Key::S)
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				if(i != _sceneNodes.begin())
				{
					--i;
					_sceneNodeId = *i;
				}
			}
			else if(key == Key::D)
			{
				++_index;
				setSample();
			}
			else if(key == Key::A)
			{
				if(_index > 0)
					--_index;
				setSample();
			}
			else if(key == Key::F2)
			{
				XmlSerializer serializer(new FileStream("C:/Users/SHLOMIATIA/Documents/Visual Studio 2010/Projects/Temporal/Temporal/Temporal/External/bin/resources/animations/aquaria.xml", true, false));
				AnimationSet& set =  *_animationSet;
				serializer.serialize("animation-set", set);
				serializer.save();
			}
		}

		void update()
		{
			std::stringstream s;
			s << _animationId.getString() << " " << _sceneNodeId.getString() << " " << _index;
			Graphics::get().setTitle(s.str().c_str());
		}

		void skeletonLeftMouseDown(const MouseParams& params)
		{
			addUndo();
			_offset = params.getPosition() - getSceneNodeSample().getTranslation();
			_translation = true;
			_rotation = false;
		}

		void skeletonRightMouseDown(const MouseParams& params)
		{
			addUndo();
			_offset = params.getPosition() - getSceneNodeSample().getTranslation();
			_translation = false;
			_rotation = true;
		}

		void skeletonMouseMove(const MouseParams& params)
		{
			if(_translation)
			{
				getSceneNodeSample().setTranslation(params.getPosition() - _offset);
				_animationSet->getAnimations().at(_animationId)->init();
			}
			else if(_rotation)
			{
				const Vector vector = params.getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getSceneNodeSample().setRotation(rotation);
				_animationSet->getAnimations().at(_animationId)->init();
			}
		}

		Panel* addPanel(const char* id, const AABB& shape)
		{
			Transform* transform = new Transform(shape.getCenter());
			Panel* panel = new Panel(shape.getRadius());
			Entity* entity = new Entity(Hash(id));
			entity->add(transform);
			entity->add(panel);
			getEntity().getManager().add(entity);
			return panel;
		}

		void addLabel(const char* label, const Vector& position, float width)
		{
			Transform* transform = new Transform(position);
			Text* text = new Text("c:/windows/fonts/Arial.ttf", 12, label);
			text->setWidth(width);
			Entity* entity = new Entity(Hash(label));
			entity->add(transform);
			entity->add(text);
			entity->handleMessage(Message(MessageID::ENTITY_INIT));
			getEntity().getManager().add(entity);
		}

		void bindSceneNodes(SceneNode& sceneNode)
		{
			_sceneNodes.push_back(sceneNode.getID());
			for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
			{
				bindSceneNodes(**i);
			}
		}

		void keyframeLeftClick(const MouseParams& params)
		{
			Panel* panel = static_cast<Panel*>(params.getSender());
			Hash id = panel->getEntity().getId();
			std::vector<std::string> parts = Utils::split(id.getString(), '.');
			_sceneNodeId = Hash(parts[0].c_str());
			_index = Utils::parseInt(parts[1].c_str());
		}

		void init()
		{
			_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
			_animationId = _animationSet->getAnimations().begin()->first;
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNodeId = *_sceneNodes.begin();
			setAnimation();

			Panel* panel = addPanel("skeletonPanel", AABB(455, 384, 512, 228));
			panel->setLeftMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonLeftMouseDown));
			panel->setRightMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonRightMouseDown));
			panel->setMouseMoveEvent(createAction1(AnimationEditor, const MouseParams&, skeletonMouseMove));

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

			y = GRID_START_Y;
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				for(int j = 0;  j < GRID_FRAMES; ++j)
				{
					std::string id = Utils::format("%s.%d", i->getString(), j); 
					Panel* panel = addPanel(id.c_str(), AABBLT(GRID_START_X + j * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					panel->setLeftMouseClickEvent(createAction1(AnimationEditor, const MouseParams&, keyframeLeftClick));
				}
				y -= CELL_SIZE;
			}
		}

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTITY_INIT)
			{
				init();
			}
			else if(message.getID() == MessageID::KEY_UP)
			{
				Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
				handleKey(key);
			}
			else if(message.getID() == MessageID::UPDATE)
			{
				update();
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
		bool _rotation;
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
				Entity* entity = new Entity(Hash("ENT_ANIMATION_EDITOR"));
				entity->add(new AnimationEditor());
				gameState.getEntitiesManager().add(entity);
			}
		}
	};
}