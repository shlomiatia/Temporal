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
			initSns();
		}

		void setSample()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_index));
		}

		void initSns()
		{
			for(int j = 0;  j < GRID_FRAMES; ++j)
			{
				SceneGraphSample* sgs = getSceneGraphSample(j);
				bool isEmptySgs = sgs && sgs->getSamples().size() == 0;
				for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
				{
					Hash id = getSnsId(*i, j); 
					Panel& panel = *static_cast<Panel*>(getEntity().getManager().getEntity(id)->get(Panel::TYPE));
					if(isEmptySgs)
					{
						panel.setFill(true);
						panel.setColor(Color::Red);
					}
					else
					{
						SceneNodeSample* sns = getSceneNodeSample(j, *i);
						panel.setFill(sns != 0);
						panel.setColor(Color::White);
					}
				}
			}
		}

		SceneGraphSample* getSceneGraphSample(int index = -1, bool createSceneGraphSample = false, bool deleteSceneGraphSample = false)
		{
			if(index == -1)
				index = _index;
			SceneGraphSampleCollection& samples = _animationSet->get(_animationId).getSamples();
			for(SceneGraphSampleIterator i = samples.begin();; ++i)
			{
				if(createSceneGraphSample && (i == samples.end() || (**i).getIndex() > index))
				{
					return *samples.insert(i, new SceneGraphSample(index));
				}
				else if(i == samples.end())
				{
					return 0;
				}
				else if((**i).getIndex() == index)
				{
					if(!deleteSceneGraphSample)
						return *i;
					delete *i;
					i = samples.erase(i);
					return 0;
				}
			}
		}

		SceneNodeSample* addSample(SceneNodeSampleCollection& samples, SceneNodeSampleIterator iterator)
		{
			const SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
			const SceneNode& node = *root.get(_sceneNodeId);
			SceneNodeSample* sample = new SceneNodeSample(_sceneNodeId);
			sample->setTranslation(node.getTranslation());
			sample->setRotation(node.getRotation());
			samples[sample->getId()] = sample;
			_animationSet->get(_animationId).init();
			return sample;
		}

		SceneNodeSample* getSceneNodeSample(int index = -1, Hash sceneNodeId = Hash::INVALID, bool createSceneNodeSample = false, bool deleteSceneNodeSample = false)
		{
			if(sceneNodeId == Hash::INVALID)
				sceneNodeId = _sceneNodeId;
			SceneGraphSample* sceneGraphSample = getSceneGraphSample(index, createSceneNodeSample);
			if(!sceneGraphSample)
				return 0;
			SceneNodeSampleCollection& samples = sceneGraphSample->getSamples();
			SceneNodeSampleIterator i = samples.find(sceneNodeId);
			if(i == samples.end())
			{
				if(!createSceneNodeSample)
					return 0;
				return addSample(samples, i);
			}
			else
			{
				if(!deleteSceneNodeSample)
					return i->second;
				delete i->second;
				i = samples.erase(i);
				if(sceneGraphSample->getSamples().size() == 0)
					getSceneGraphSample(index, false, true);
				return 0;
			}
		}

		SceneNodeSample& getCreateSceneNodeSample(int index = -1, Hash sceneNodeId = Hash::INVALID)
		{
			return *getSceneNodeSample(index, sceneNodeId, true);
		}

		void addUndo()
		{
			if(_undo)
				delete _undo;
			_undo = _animationSet->clone();
		}

		void paste()
		{
			addUndo();
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				SceneNodeSampleCollection* sceneNodeSamples = 0;
				SceneNodeSample* copy = getSceneNodeSample(_copyIndex, *i);
				if(!copy)
					continue;
				SceneNodeSample& paste = getCreateSceneNodeSample(_index, *i);
				paste.setTranslation(copy->getTranslation());
				paste.setRotation(copy->getRotation());
			}
		}

		void deleteSample()
		{
			if(_index == 0)
				return;
			addUndo();
			getSceneNodeSample(_index, _sceneNodeId, false, true);
		}

		void handleArrows(const Vector& vector)
		{
			SceneNodeSample& sample = getCreateSceneNodeSample();
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
				XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/animations/aquaria.xml", true, false));
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
			_offset = params.getPosition() - getCreateSceneNodeSample().getTranslation();
			_translation = true;
			_rotation = false;
		}

		void skeletonRightMouseDown(const MouseParams& params)
		{
			addUndo();
			_offset = params.getPosition() - getCreateSceneNodeSample().getTranslation();
			_translation = false;
			_rotation = true;
		}

		void skeletonMouseMove(const MouseParams& params)
		{
			if(_translation)
			{
				getCreateSceneNodeSample().setTranslation(params.getPosition() - _offset);
				_animationSet->getAnimations().at(_animationId)->init();
			}
			else if(_rotation)
			{
				const Vector vector = params.getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getCreateSceneNodeSample().setRotation(rotation);
				_animationSet->getAnimations().at(_animationId)->init();
			}
		}

		Panel* addPanel(Hash id, const AABB& shape)
		{
			Transform* transform = new Transform(shape.getCenter());
			Panel* panel = new Panel(shape.getRadius());
			Entity* entity = new Entity(id);
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

		void snsLeftClick(const MouseParams& params)
		{
			Panel* panel = static_cast<Panel*>(params.getSender());
			Hash id = panel->getEntity().getId();
			std::vector<std::string> parts = Utils::split(id.getString(), '.');
			_sceneNodeId = Hash(parts[0].c_str());
			_index = Utils::parseInt(parts[1].c_str());
			setSample();
		}

		Hash getSnsId(Hash snId, int index)
		{
			std::string sid = Utils::format("%s.%d", snId.getString(), index);
			return Hash(sid.c_str());
		}

		void init()
		{
			_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
			_animationId = _animationSet->getAnimations().begin()->first;
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNodeId = *_sceneNodes.begin();

			Panel* panel = addPanel(Hash("skeletonPanel"), AABB(455, 384, 512, 228));
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
					Hash id = getSnsId(*i, j); 
					Panel* panel = addPanel(id, AABBLT(GRID_START_X + j * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					panel->setLeftMouseClickEvent(createAction1(AnimationEditor, const MouseParams&, snsLeftClick));
				}
				y -= CELL_SIZE;
			}
			setAnimation();
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