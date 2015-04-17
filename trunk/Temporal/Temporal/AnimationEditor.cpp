#include "AnimationEditor.h"
#include "GameState.h"
#include "Mouse.h"
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
#include <algorithm>
#include <sstream>

namespace Temporal
{
	void AnimationEditor::setAnimation(Hash animationId)
	{
		_animationId = animationId;
		getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::RESET_ANIMATION, &AnimationParams(_animationId)));
		setFrame();
		updateGrid();
	}

	void AnimationEditor::setFrame()
	{
		if(_paused)
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_ANIMATION_FRAME, &_index));
	}

	void AnimationEditor::setSceneGraphIndex(int index)
	{
		_index = index;
		updateGrid();
	}

	void AnimationEditor::setSceneNodeId(Hash sceneNodeId)
	{
		_sceneNodeId = sceneNodeId;
		updateGrid();
	}

	void AnimationEditor::updateGrid()
	{
		for(int i = 0;  i < GRID_FRAMES; ++i)
		{
			SceneGraphSample* sgs = getSceneGraphSample(i);
			bool isEmptySgs = sgs && sgs->getSamples().size() == 0;
			for(HashIterator j = _sceneNodes.begin(); j != _sceneNodes.end(); ++j)
			{
				if(*j == Hash("SCN_ROOT"))
					continue;
				Hash id = getSnsId(i, *j);

				Control* control = static_cast<Control*>(getEntity().getManager().getEntity(id)->get(Control::TYPE));
				if(!control)
					continue;
				SceneNodeSample* sns = getSceneNodeSample(i, *j);
				if(_index == i && _sceneNodeId == *j)
				{
					control->setBackgroundColor(sns == 0 ? Color(1.0f ,0.65f ,0) : Color::Yellow);
				}
				else if(isEmptySgs)
				{
					control->setBackgroundColor(Color::Red);
				}
				else
				{
					control->setBackgroundColor(sns != 0 ? Color::White : Color::Transparent);
				}
			}
		}
	}

	void AnimationEditor::bindSceneNodes(SceneNode& sceneNode)
	{
		_sceneNodes.push_back(sceneNode.getID());
		for(SceneNodeIterator i = sceneNode.getChildren().begin(); i != sceneNode.getChildren().end(); ++i)
		{
			bindSceneNodes(**i);
		}
	}

	Hash AnimationEditor::getSnsId(int index, Hash snId)
	{
		std::string sid = Utils::format("%d.%s", index, snId.getString());
		return Hash(sid.c_str());
	}

	SceneGraphSample* AnimationEditor::getSceneGraphSample(int index, SceneNodeAction::Enum action, Hash animationId)
	{
		if(index == -1)
			index = _index;
		if(animationId == Hash::INVALID)
			animationId = _animationId;
		SceneGraphSampleCollection& samples = _animationSet->get(animationId).getSamples();
		for(SceneGraphSampleIterator i = samples.begin();; ++i)
		{
			if(action == SceneNodeAction::CREATE && (i == samples.end() || (**i).getIndex() > index))
			{
				SceneGraphSample* result = *samples.insert(i, new SceneGraphSample(index));
				_animationSet->get(animationId).init();
				updateGrid();
				return result;
			}
			else if(i == samples.end())
			{
				return 0;
			}
			else if((**i).getIndex() == index)
			{
				if(action != SceneNodeAction::DELETE)
					return *i;
				delete *i;
				i = samples.erase(i);
				_animationSet->get(animationId).init();
				updateGrid();
				return 0;
			}
		}
	}

	SceneNodeSample* AnimationEditor::getSceneNodeSample(int index, Hash sceneNodeId, SceneNodeAction::Enum action, Hash animationId)
	{
		if(sceneNodeId == Hash::INVALID)
			sceneNodeId = _sceneNodeId;
		if(animationId == Hash::INVALID)
			animationId = _animationId;
		SceneGraphSample* sceneGraphSample = getSceneGraphSample(index, action == SceneNodeAction::CREATE ? action : SceneNodeAction::NONE, animationId);
		if(!sceneGraphSample)
			return 0;
		HashSceneNodeSampleMap& samples = sceneGraphSample->getSamples();
		SceneNodeSampleIterator i = samples.find(sceneNodeId);
		if(action == SceneNodeAction::DELETE)
		{
			if(i != samples.end())
			{
				delete i->second;
				i = samples.erase(i);
			}
			if(sceneGraphSample->getSamples().size() == 0)
				getSceneGraphSample(index, action);
			_animationSet->get(animationId).init();
			updateGrid();
			return 0;
		}
		else if(i == samples.end())
		{
			if(action != SceneNodeAction::CREATE)
				return 0;
			SceneNodeSample* result = addSample(samples, sceneNodeId);
			_animationSet->get(animationId).init();
			updateGrid();
			return result;
		}
		else
		{
			return i->second;
		}
	}

	SceneNodeSample& AnimationEditor::getCreateSceneNodeSample(int index, Hash sceneNodeId, Hash animationId)
	{
		return *getSceneNodeSample(index, sceneNodeId, SceneNodeAction::CREATE, animationId);
	}

	SceneNodeSample* AnimationEditor::addSample(HashSceneNodeSampleMap& samples, Hash sceneNodeId)
	{
		const SceneNode& node = getSceneNode(sceneNodeId);
		SceneNodeSample* sample = new SceneNodeSample(sceneNodeId);
		sample->setTranslation(node.getTranslation());
		sample->setRotation(node.getRotation());
		samples[sample->getId()] = sample;
		return sample;
	}

	void AnimationEditor::handleArrows(const Vector& vector)
	{
		SceneNodeSample& sample = getCreateSceneNodeSample();
		if(_translation)
		{
			Vector translation = getTranslation(vector);
			sample.setTranslation(sample.getTranslation() + translation);
		}
		else
		{
			float angle = sample.getRotation() + vector.getX() + vector.getY();
			setRotation(angle);
		}
	}

	void AnimationEditor::addUndo()
	{
		_undo.push_back(_animationSet->clone());
	}

	void AnimationEditor::undo()
	{
		if(_undo.size() > 0)
		{
			std::shared_ptr<AnimationSet> undo(_undo[_undo.size() - 1]);
			_undo.pop_back();
			ResourceManager::get()._animationSets[Hash("resources/animations/aquaria.xml")] = undo;
			undo->init();
			_animationSet = undo;
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::ENTITY_INIT));
			updateGrid();
		}
	}

	void AnimationEditor::save()
	{
		XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/animations/aquaria.xml", true, false));
		AnimationSet& set =  *_animationSet;
		serializer.serialize("animation-set", set);
		serializer.save();
	}

	void AnimationEditor::newAnimation(const char* name)
	{
		addUndo();
		Animation* animation = _animationSet->get(_animationId).clone();
		Hash animationId = Hash(name);
		animation->setId(animationId);
		animation->setRepeat(true);
		animation->init();
		_animationSet->add(animation);
		setAnimation(animationId);
	}

	void AnimationEditor::deleteAnimation()
	{
		addUndo();
		Hash id = _animationId;
		previousAnimation();
		_animationSet->remove(id);
	}

	void AnimationEditor::nextAnimation()
	{
		AnimationIterator i = _animationSet->getAnimations().find(_animationId);
		++i;
		if(i != _animationSet->getAnimations().end())
			setAnimation(i->first);
	}

	void AnimationEditor::previousAnimation()
	{
		AnimationIterator i = _animationSet->getAnimations().find(_animationId);
		if(i != _animationSet->getAnimations().begin())
		{
			--i;
			setAnimation(i->first);
		}
	}

	void AnimationEditor::toggleAnimation()
	{
		_paused = !_paused;
		getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::TOGGLE_ANIMATION, &_paused));
		setFrame();
	}

	void AnimationEditor::copy()
	{
		_copyAnimation = _animationId;
		_copyIndex = _index;
	}

	void AnimationEditor::paste()
	{
		addUndo();
		for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			HashSceneNodeSampleMap* sceneNodeSamples = 0;
			SceneNodeSample* copy = getSceneNodeSample(_copyIndex, *i, SceneNodeAction::NONE, _copyAnimation);
			if(!copy)
				continue;
			SceneNodeSample& paste = getCreateSceneNodeSample(_index, *i);
			paste.setTranslation(copy->getTranslation());
			paste.setRotation(copy->getRotation());
		}
	}

	void AnimationEditor::newSgs()
	{
		getSceneGraphSample(_index, SceneNodeAction::CREATE);
	}

	void AnimationEditor::deleteSns()
	{
		if(_index == 0)
			return;
		addUndo();
		getSceneNodeSample(_index, _sceneNodeId, SceneNodeAction::DELETE);
	}

	void AnimationEditor::handleKey(Key::Enum key)
	{
		if(key == Key::UP)
		{
			handleArrows(Vector(0.0f, 1.0f));
		}
		else if(key == Key::DOWN)
		{
			handleArrows(Vector(0.0f, -1.0f));
		}
		else if(key == Key::BACKSLASH)
		{
			handleArrows(Vector(0.0f, 10.0f));
		}
		else if(key == Key::SLASH)
		{
			handleArrows(Vector(0.0f, -10.0f));
		}
		else if(key == Key::LEFT)
		{
			handleArrows(Vector(-1.0f, 0.0f));
		}
		else if(key == Key::RIGHT)
		{
			handleArrows(Vector(1.0f, 0.0f));
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
			setSceneGraphIndex(_index + 1);
			setFrame();
		}
		else if(key == Key::A)
		{
			if(_index > 0)
				setSceneGraphIndex(_index - 1);
			setFrame();
		}
	}

	void AnimationEditor::init()
	{
		const float CELL_SIZE = 16.0f;
		const float PADDING = 16.0f;
		const Vector PADDING_VECTOR(PADDING, PADDING);
		const Vector WINDOW_SIZE(Graphics::get().getLogicalView());
		const Vector BUTTON_SIZE(WINDOW_SIZE.getX() / 4.0f - PADDING * 2.0f, 32.0f);
		const Vector PADDED_BUTTON_SIZE(BUTTON_SIZE + PADDING_VECTOR  * 2.0f);
		const Vector PADDED_PANEL_SIZE(WINDOW_SIZE / 2.0f);
		const Vector PANEL_SIZE(PADDED_PANEL_SIZE.getX(), PADDED_PANEL_SIZE.getY() - PADDING * 2.0f);
		const float GRID_START_Y = PADDED_PANEL_SIZE.getY() - CELL_SIZE;
		GRID_FRAMES = (WINDOW_SIZE.getX() - PADDED_BUTTON_SIZE.getX()) / CELL_SIZE;
			
		_animationSet = ResourceManager::get().getAnimationSet("resources/animations/aquaria.xml");
		
		SceneNode& sceneNode = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_ROOT_SCENE_NODE)));
		bindSceneNodes(sceneNode);

		Control* control = addControl(Hash("skeletonPanel"), AABBLT(PADDED_BUTTON_SIZE.getX(), WINDOW_SIZE.getY() - PADDING, PANEL_SIZE.getX(), PANEL_SIZE.getY()));
		control->setBackgroundColor(Color::Transparent);
		control->setLeftMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonLeftMouseDown));
		control->setMiddleMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonMiddleMouseDown));
		control->setRightMouseDownEvent(createAction1(AnimationEditor, const MouseParams&, skeletonRightMouseDown));
		control->setMouseMoveEvent(createAction1(AnimationEditor, const MouseParams&, skeletonMouseMove));

		AABB bounds(PADDING + BUTTON_SIZE.getX() / 2.0f, WINDOW_SIZE.getY() - PADDING - BUTTON_SIZE.getY() / 2.0f, BUTTON_SIZE.getX(), BUTTON_SIZE.getY());
		control = addButton(Hash("saveAnimationSet"), bounds, "Save Animation Set", createAction(AnimationEditor, save), Key::F2);
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addTextBox(Hash("newAnimation"), bounds, "New Animation", createAction1(AnimationEditor, const char*, newAnimation));
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("deleteAnimation"), bounds, "Delete Animation", createAction(AnimationEditor, deleteAnimation));
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		AABB halfBounds(PADDING + (BUTTON_SIZE.getX() - PADDING) / 4.0f, bounds.getCenterY(), (BUTTON_SIZE.getX() - PADDING) / 2.0f, BUTTON_SIZE.getY());
		control = addButton(Hash("nextAnimation"), halfBounds, "Next Animation", createAction(AnimationEditor, nextAnimation), Key::PAGE_UP);
		halfBounds.setCenterX(halfBounds.getCenterX() + (BUTTON_SIZE.getX() + PADDING) / 2.0f);
		control = addButton(Hash("previousAnimation"), halfBounds, "Prev Animation", createAction(AnimationEditor, previousAnimation), Key::PAGE_DOWN);
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("toggleAnimation"), bounds, "Toggle Animation", createAction(AnimationEditor, toggleAnimation), Key::SPACE);

		bounds = AABB(PADDED_BUTTON_SIZE.getX() + PADDED_PANEL_SIZE.getX() + PADDING + BUTTON_SIZE.getX() / 2.0f, WINDOW_SIZE.getY() - PADDING - BUTTON_SIZE.getY() / 2.0f, BUTTON_SIZE.getX(), BUTTON_SIZE.getY());
		control = addButton(Hash("newSgs"), bounds, "New Sgs", createAction(AnimationEditor, newSgs), Key::N);		
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("deleteSs"), bounds, "Delete Sns", createAction(AnimationEditor, deleteSns), Key::DEL);
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("copySgs"), bounds, "Copy Sgs", createAction(AnimationEditor, copy), Key::C);
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("pasteSgs"), bounds, "Paste Sgs", createAction(AnimationEditor, paste), Key::V);
		bounds.setCenterY(bounds.getCenterY() - BUTTON_SIZE.getY() - PADDING);
		control = addButton(Hash("undo"), bounds, "Undo", createAction(AnimationEditor, undo), Key::Z);

		float y = PADDED_PANEL_SIZE.getY();
		addControl(Hash::INVALID, AABBLT(PADDING, y, PADDED_BUTTON_SIZE.getX(), CELL_SIZE));
		for(HashIterator i = _sceneNodes.begin(); i != _sceneNodes.end(); ++i)
		{
			if(*i == Hash("SCN_ROOT"))
				continue;
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
				if(*j == Hash("SCN_ROOT"))
					continue;
				Hash id = getSnsId(i, *j); 
				Control* control = addControl(id, AABBLT(PADDED_BUTTON_SIZE.getX() + i * CELL_SIZE, y, CELL_SIZE, CELL_SIZE));
				control->setLeftMouseClickEvent(createAction1(AnimationEditor, const MouseParams&, snsLeftClick));
				y -= CELL_SIZE;
			}
				
		}
		setSceneGraphIndex(0);
		setSceneNodeId(*(_sceneNodes.begin()+1));
	}

	void AnimationEditor::update()
	{
		SceneNodeSample* sample = getSceneNodeSample();
		std::stringstream s;
		s << "[Animation: " << _animationId.getString() << "][Frame: " << _index << "][Node: " << _sceneNodeId.getString() << "]";
		if(sample)
			s << "[X: " << sample->getTranslation().getX() << "][Y: " << sample->getTranslation().getY() << "][Angle: " << sample->getRotation() << "]";
		Graphics::get().setTitle(s.str().c_str());
	}

	const SceneNode& AnimationEditor::getSceneNode(Hash sceneNodeId)
	{
		if(sceneNodeId == Hash::INVALID)
			sceneNodeId = _sceneNodeId;
		SceneNode& root = *static_cast<SceneNode*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), (Message(MessageID::GET_ROOT_SCENE_NODE))));
		const SceneNode& node = *root.get(_sceneNodeId);
		return node;
	}

	Vector AnimationEditor::getParentGlobalPosition()
	{
		Vector position = getVectorParam(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_POSITION)));
		const SceneNode& node = getSceneNode().parent();
		Matrix globalMatrix = node.getGlobalMatrix();
		Vector globalTranslation = globalMatrix * Vector::Zero;
		position += globalTranslation;
		return position;
	}

	Vector AnimationEditor::getGlobalPosition()
	{
		Vector position = getVectorParam(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_POSITION)));
		const SceneNode& node = getSceneNode();
		Matrix globalMatrix = node.getGlobalMatrix();
		Vector globalTranslation = globalMatrix * Vector::Zero;
		position += globalTranslation;
		return position;
	}

	void AnimationEditor::setRotation(float rotation)
	{
		const SceneNode& node = getSceneNode();
		if(AngleUtils::degree().minDistance(rotation, node.getCenter()) <= node.getRadius())
			getCreateSceneNodeSample().setRotation(rotation);
	}

	Vector AnimationEditor::getTranslation(const Vector& translation)
	{
		const SceneNode& node = getSceneNode();
		Matrix globalMatrix = node.parent().getGlobalMatrix();
		globalMatrix.resetTranslation();
		globalMatrix.inverse();
		return globalMatrix * translation;
	}

	void AnimationEditor::skeletonLeftMouseDown(const MouseParams& params)
	{
		addUndo();
		_offsetTranslation = params.getPosition() - getGlobalPosition();
		_translation = true;
		_rotation = false;
		_move = false;
	}

	void AnimationEditor::skeletonMiddleMouseDown(const MouseParams& params)
	{
		Vector position = getVectorParam(getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::GET_POSITION)));
		_offsetTranslation = params.getPosition() - position;
		_translation = false;
		_rotation = false;
		_move = true;
	}

	void AnimationEditor::skeletonRightMouseDown(const MouseParams& params)
	{
		addUndo();
		Vector position = getGlobalPosition();
		const SceneNode& node = getSceneNode();
		_offsetAngle = (params.getPosition() - position).getAngle() - AngleUtils::degreesToRadians(node.getRotation());
		_translation = false;
		_rotation = true;
		_move = false;
	}

	void AnimationEditor::skeletonMouseMove(const MouseParams& params)
	{
		if(_translation)
		{
			
			Vector translation = params.getPosition() - _offsetTranslation - getParentGlobalPosition();
			translation = getTranslation(translation);
			getCreateSceneNodeSample().setTranslation(translation);
		}
		else if(_rotation)
		{
		
			Vector position = getGlobalPosition();
			const SceneNode& node = getSceneNode();
			Vector vector = params.getPosition() - position;
			float angle = AngleUtils::radian().normalize(vector.getAngle() - _offsetAngle);
			float rotation = AngleUtils::radiansToDegrees(angle);
			setRotation(rotation);
		}
		else if(_move)
		{
			Vector position = params.getPosition() - _offsetTranslation;
			getEntity().getManager().sendMessageToEntity(Hash("ENT_SKELETON"), Message(MessageID::SET_POSITION, &position));
		}
	}

	void AnimationEditor::snsLeftClick(const MouseParams& params)
	{
		Control* panel = static_cast<Control*>(params.getSender());
		Hash id = panel->getEntity().getId();
		std::vector<std::string> parts = Utils::split(id.getString(), '.');
		setSceneGraphIndex(Utils::parseInt(parts[0].c_str()));
		setSceneNodeId(Hash(parts[1].c_str()));
		setFrame();
	}

	Control* AnimationEditor::addControl(Hash id, const AABB& shape)
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

	Control* AnimationEditor::addLabel(const AABB& shape, const char* text)
	{
		Control* control = addControl(Hash(text), shape);
		control->setText(text);
		return control;
	}

	Control* AnimationEditor::addButton(Hash id, const AABB& shape, const char* text, IAction* commandEvent, Key::Enum shortcutKey)
	{
		Control* control = addControl(id, shape);
		control->setText(text);
		control->setCommandEvent(commandEvent);
		control->setShortcutKey(shortcutKey);
		return control;
	}

	Control* AnimationEditor::addTextBox(Hash id, const AABB& shape, const char* text, IAction1<const char*>* textChangedEvent)
	{
		Control* control = addControl(id, shape);
		control->setText(text);
		control->setTextChangedEvent(textChangedEvent);
		return control;
	}

	void AnimationEditor::handleMessage(Message& message)
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
			setAnimation(_animationSet->getAnimations().begin()->first);
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

	void AEGameStateListener::onLoaded(Hash id, GameState& gameState)
	{
		if(id == Hash("resources/game-states/animation-editor.xml"))
		{
			Entity* entity = new Entity(Hash("ENT_ANIMATION_EDITOR"));
			entity->add(new AnimationEditor());
			gameState.getEntitiesManager().add(entity);
		}
	}
}