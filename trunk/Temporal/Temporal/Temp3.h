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
#include "Control.h"
#include "Transform.h"
#include "Utils.h"
#include "Graphics.h"
#include "Shapes.h"
#include "Layer.h"
#include "MessageUtils.h"
#include "Delegate.h"
#include <algorithm>
#include <sstream>

// Animation editor
namespace Temporal
{
	class AnimationEditor : public Component
	{
	public:
		AnimationEditor() : _offset(Vector::Zero), _translation(false), _rotation(false), _index(0), _copyIndex(0), _undo(0), GRID_FRAMES(0) {}

		Hash getType() const { return Hash::INVALID; }

		void setAnimation(int index = 0)
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &_animationId));
			setIndex(index);
			setSample();
			initSns();
		}

		void setSample()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_index));
		}

		void initSns()
		{
			for(int i = 0;  i < GRID_FRAMES; ++i)
			{
				SceneGraphSample* sgs = getSceneGraphSample(i);
				bool isEmptySgs = sgs && sgs->getSamples().size() == 0;
				for(HashIterator j = _sceneNodes.begin(); j != _sceneNodes.end(); ++j)
				{
					Hash id = getSnsId(i, *j);
					Control& control = *static_cast<Control*>(getEntity().getManager().getEntity(id)->get(Control::TYPE));
					SceneNodeSample* sns = getSceneNodeSample(i, *j);
					if(_index == i && _sceneNodeId == *j)
					{
						control.setBackgroundColor(sns == 0 ? Color(1.0f ,0.65f ,0) : Color::Yellow);
					}
					else if(isEmptySgs)
					{
						control.setBackgroundColor(Color::Red);
					}
					else
					{
						control.setBackgroundColor(sns != 0 ? Color::White : Color::Transparent);
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
					SceneGraphSample* result = *samples.insert(i, new SceneGraphSample(index));
					_animationSet->get(_animationId).init();
					initSns();
					return result;
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
			if(deleteSceneNodeSample)
			{
				if(i != samples.end())
				{
					delete i->second;
					i = samples.erase(i);
				}
				if(sceneGraphSample->getSamples().size() == 0)
					getSceneGraphSample(index, false, true);
				_animationSet->get(_animationId).init();
				initSns();
				return 0;
			}
			else if(i == samples.end())
			{
				if(!createSceneNodeSample)
					return 0;
				SceneNodeSample* result = addSample(samples, i);
				_animationSet->get(_animationId).init();
				initSns();
				return result;
			}
			else
			{
				return i->second;
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

		void deleteSns()
		{
			if(_index == 0)
				return;
			addUndo();
			getSceneNodeSample(_index, _sceneNodeId, false, true);
			setAnimation(_index);
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

		void setSceneNodeId(Hash sceneNodeId)
		{
			_sceneNodeId = sceneNodeId;
			initSns();
		}

		void setIndex(int index)
		{
			_index = index;
			initSns();
		}

		void nextAnimation()
		{
			AnimationIterator i = _animationSet->getAnimations().find(_animationId);
			++i;
			if(i != _animationSet->getAnimations().end())
				_animationId = i->first;
			setAnimation();
		}

		void previousAnimation()
		{
			AnimationIterator i = _animationSet->getAnimations().find(_animationId);
			if(i != _animationSet->getAnimations().begin())
			{
				--i;
				_animationId = i->first;
			}
			setAnimation();
		}

		void toggleAnimation()
		{
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::TOGGLE_ANIMATION));
		}

		void undo()
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

		void copy()
		{
			_copyIndex = _index;
		}

		void newSgs()
		{
			getSceneGraphSample(_index, true);
		}

		void save()
		{
			XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/animations/aquaria.xml", true, false));
			AnimationSet& set =  *_animationSet;
			serializer.serialize("animation-set", set);
			serializer.save();
		}

		void handleKey(Key::Enum key)
		{
			if(key == Key::UP)
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
			else if(key == Key::Z)
			{
				undo();	
			}
			else if(key == Key::C)
			{
				copy();
			}
			else if(key == Key::V)
			{
				paste();
			}
			else if(key == Key::DELETE)
			{
				deleteSns();
			}
			else if(key == Key::N)
			{
				newSgs();
			}
			else if(key == Key::S)
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				++i;
				if(i != _sceneNodes.end())
					setSceneNodeId(*i);
			}
			else if(key == Key::W)
			{
				HashIterator i =  std::find(_sceneNodes.begin(), _sceneNodes.end(), _sceneNodeId);
				if(i != _sceneNodes.begin())
				{
					--i;
					setSceneNodeId(*i);
				}
			}
			else if(key == Key::D)
			{
				setIndex(_index + 1);
				setSample();
			}
			else if(key == Key::A)
			{
				if(_index > 0)
					setIndex(_index - 1);
				setSample();
			}
			else if(key == Key::F2)
			{
				save();
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
				//_animationSet->getAnimations().at(_animationId)->init();
			}
			else if(_rotation)
			{
				const Vector vector = params.getPosition() - _offset;
				float rotation = fromRadians(vector.getAngle());
				getCreateSceneNodeSample().setRotation(rotation);
				//_animationSet->getAnimations().at(_animationId)->init();
			}
		}

		Control* addControl(Hash id, const AABB& shape)
		{		
			Transform* transform = new Transform(shape.getCenter());
			Control* control = new Control();
			control->setWidth(shape.getWidth());
			control->setHeight(shape.getHeight());
			Entity* entity = new Entity(id);
			entity->add(transform);
			entity->add(control);
			getEntity().getManager().add(entity);
			return control;
		}

		Control* addLabel(const AABB& shape, const char* text)
		{
			Control* control = addControl(Hash(text), shape);
			control->setText(text);
			return control;
		}

		Control* addButton(Hash id, const AABB& shape, const char* text, IAction* commandEvent, Key::Enum shortcutKey = Key::NONE)
		{
			Control* control = addControl(id, shape);
			control->setText(text);
			control->setCommandEvent(commandEvent);
			control->setShortcutKey(shortcutKey);
			return control;
		}

		Control* addTextBox(Hash id, const AABB& shape, const char* text, IAction1<const char*>* textChangedEvent)
		{
			Control* control = addControl(id, shape);
			control->setText(text);
			control->setTextChangedEvent(textChangedEvent);
			return control;
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
			Control* panel = static_cast<Control*>(params.getSender());
			Hash id = panel->getEntity().getId();
			std::vector<std::string> parts = Utils::split(id.getString(), '.');
			setIndex(Utils::parseInt(parts[0].c_str()));
			setSceneNodeId(Hash(parts[1].c_str()));
			setSample();
		}

		Hash getSnsId(int index, Hash snId)
		{
			std::string sid = Utils::format("%d.%s", index, snId.getString());
			return Hash(sid.c_str());
		}

		void newAnimation(const char* name)
		{
			addUndo();
			Animation* animation = _animationSet->get(_animationId).clone();
			_animationId = Hash(name);
			animation->setId(_animationId);
			animation->setRepeat(true);
			animation->init();
			_animationSet->add(animation);
			setAnimation();
		}

		void deleteAnimation()
		{
			addUndo();
			Hash id = _animationId;
			previousAnimation();
			_animationSet->remove(id);
		}

		void init()
		{
			const float CELL_SIZE = 16.0f;
			const float PADDING = 16.0f;
			const Vector PADDING_VECTOR(PADDING, PADDING);
			const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
			const Vector BUTTON_SIZE(WINDOW_SIZE.getX() / 4.0f - PADDING * 2.0f, 32.0f);
			const Vector PADDED_BUTTON_SIZE(BUTTON_SIZE + PADDING_VECTOR  * 2.0f);
			const Vector PADDED_PANEL_SIZE(WINDOW_SIZE / 2.0f);
			const Vector PANEL_SIZE(PADDED_PANEL_SIZE.getX(), PADDED_PANEL_SIZE.getY() - PADDING);
			const float GRID_START_Y = PADDED_PANEL_SIZE.getY() - CELL_SIZE;
			GRID_FRAMES = (WINDOW_SIZE.getX() - PADDED_BUTTON_SIZE.getX()) / CELL_SIZE;
			
			_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
			_animationId = _animationSet->getAnimations().begin()->first;
			SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
			bindSceneNodes(sceneNode);
			_sceneNodeId = *_sceneNodes.begin();

			Control* control = addControl(Hash("skeletonPanel"), AABBLT(PADDED_BUTTON_SIZE.getX(), WINDOW_SIZE.getY() - PADDING, PANEL_SIZE.getX(), PANEL_SIZE.getY()));
			control->setBackgroundColor(Color::Transparent);
			control->setLeftMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonLeftMouseDown));
			control->setRightMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonRightMouseDown));
			control->setMouseMoveEvent(createAction1(AnimationEditor, const MouseParams&, skeletonMouseMove));

			Vector position(PADDING + BUTTON_SIZE.getX() / 2.0f, WINDOW_SIZE.getY() - PADDING - BUTTON_SIZE.getY() / 2.0f);
			control = addTextBox(Hash("newAnimation"), AABB(position, BUTTON_SIZE / 2.0f), "New Animation", createAction1(AnimationEditor, const char*, newAnimation));
			position.setY(position.getY() - BUTTON_SIZE.getY() - PADDING);
			control = addButton(Hash("deleteAnimation"), AABB(position, BUTTON_SIZE / 2.0f), "Delete Animation", createAction(AnimationEditor, deleteAnimation));
			position.setY(position.getY() - BUTTON_SIZE.getY() - PADDING);
			control = addButton(Hash("nextAnimation"), AABB(position, BUTTON_SIZE / 2.0f), "Next Animation", createAction(AnimationEditor, nextAnimation), Key::PAGE_UP);
			position.setY(position.getY() - BUTTON_SIZE.getY() - PADDING);
			control = addButton(Hash("previousAnimation"), AABB(position, BUTTON_SIZE / 2.0f), "Previous Animation", createAction(AnimationEditor, previousAnimation), Key::PAGE_DOWN);
			position.setY(position.getY() - BUTTON_SIZE.getY() - PADDING);
			control = addButton(Hash("toggleAnimation"), AABB(position, BUTTON_SIZE / 2.0f), "Toggle Animation", createAction(AnimationEditor, toggleAnimation), Key::SPACE);

			//control = addControl(Hash("button2"), AABBLT(PADDED_BUTTON_SIZE.getX() + PADDED_PANEL_SIZE.getX() + PADDING, WINDOW_SIZE.getY() - PADDING, BUTTON_SIZE.getX(), BUTTON_SIZE.getY()));

			float y = PADDED_PANEL_SIZE.getY();
			addControl(Hash::INVALID, AABBLT(PADDING, y, PADDED_BUTTON_SIZE.getX(), CELL_SIZE));
			for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
			{
				y -= CELL_SIZE;
				addLabel(AABBLT(PADDING, y, PADDED_BUTTON_SIZE.getX() - PADDING, CELL_SIZE), i->getString());
			}
			
			for(int i = 0;  i < GRID_FRAMES; ++i)
			{
				addLabel(AABBLT(PADDED_BUTTON_SIZE.getX() + i * CELL_SIZE, PADDED_PANEL_SIZE.getY(), CELL_SIZE, CELL_SIZE), Utils::toString(i+1).c_str());
			}

			for(int i = 0;  i < GRID_FRAMES; ++i)
			{
				float y = GRID_START_Y;
				for(HashIterator j = _sceneNodes.begin(); j != _sceneNodes.end(); ++j)
				{
					Hash id = getSnsId(i, *j); 
					Control* control = addControl(id, AABBLT(PADDED_BUTTON_SIZE.getX() + i * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
					control->setLeftMouseClickEvent(createAction1(AnimationEditor, const MouseParams&, snsLeftClick));
					y -= CELL_SIZE;
				}
				
			}
		}

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::ENTITY_PRE_INIT)
			{
				init();
			}
			else if(message.getID() == MessageID::ENTITY_INIT)
			{
				Keyboard::get().add(this);
			}
			else if(message.getID() == MessageID::ENTITY_POST_INIT)
			{
				setAnimation();
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

		int GRID_FRAMES;

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

		void onDraw()
		{
			Graphics::get().draw(AABB(Graphics::get().getLogicalView() / 2.0f, Graphics::get().getLogicalView()), Color(0.933f,0.933f,0.933f), Color(0.8f,0.8f,0.8f));
		}
	};
}