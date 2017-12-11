#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Camera.h"
#include "MessageUtils.h"
#include "Utils.h"
#include "Mouse.h"
#include "ToolsUtils.h"
#include "DebugLayer.h"
#include "TemporalPeriod.h"
#include "LaserEditor.h"
#include "LightEditor.h"
#include "DoorEditor.h"
#include "TemporalPeriodEditor.h"
#include "PatrolEditor.h"
#include "ButtonEditor.h"
#include "ActivationSensorEditor.h"
#include "ActivationSensor.h"
#include "Button.h"
#include "Lighting.h"
#include "Laser.h"
#include "Door.h"
#include "Patrol.h"
#include "PlayerPeriod.h"
#include <sstream>


namespace Temporal
{
	const Hash PLAYER_ID("ENT_PLAYER");
	const Hash CURSOR_ENTITY_ID("ENT_CURSOR");
	const HashList TRANSLATION_ONLY_EDITABLE_FILTER({ Hash("dynamic-body"), Hash("camera-control"), Hash("light") });
	const HashList EDITABLE_FILTER({ Hash("static-body"), Hash("renderer") });
	const Hash TRANSFORM = Hash("transform");

	void addEditableToEntity(Entity& entity, GameStateEditor& editor)
	{
		for (HashIterator i = TRANSLATION_ONLY_EDITABLE_FILTER.begin(); i != TRANSLATION_ONLY_EDITABLE_FILTER.end(); ++i)
		{
			if (entity.get(*i))
			{
				entity.add(new Editable(true, editor));
				return;
			}
				
		}
		for (HashIterator i = EDITABLE_FILTER.begin(); i != EDITABLE_FILTER.end(); ++i)
		{
			if (entity.get(*i))
			{
				entity.add(new Editable(false, editor));
				return;
			}
		}
	}

	void GameStateEditor::update(float framePeriod)
	{
		float movement = (Keyboard::get().getKey(Key::LEFT_SHIFT) ? 40.0f : 20.0f) * framePeriod * 60.0f;
		if (Keyboard::get().getKey(Key::W))
		{
			moveCamera(Vector(0.0f, movement));
		}
		if (Keyboard::get().getKey(Key::A))
		{
			moveCamera(Vector(-movement, 0.0f));
		}
		if (Keyboard::get().getKey(Key::S))
		{
			moveCamera(Vector(0.0f, -movement));
		}
		if (Keyboard::get().getKey(Key::D))
		{
			moveCamera(Vector(movement, 0.0f));
		}
		if (getEntity().getManager().getFocusInputComponent() == this)
		{
			OBB shape = Editable::getShape(*getEntity().getManager().getEntity(CURSOR_ENTITY_ID));
			float tileSize = getEntity().getManager().getGameState().getGrid().getTileSize();
			Vector position = Mouse::get().getOffsetPosition();
			position.setX(ToolsUtils::snap(position.getX(), tileSize / 4.0f, 8.0f, shape.getRadiusX()));
			position.setY(ToolsUtils::snap(position.getY(), tileSize / 4.0f, 8.0f, shape.getRadiusY()));
			getEntity().getManager().sendMessageToEntity(CURSOR_ENTITY_ID, Message(MessageID::SET_POSITION, &position));
		}
		_autoSaveTimer.update(framePeriod);
		if (_autoSaveTimer.getElapsedTime() > 120.0f)
		{
			save();
			getEntity().getManager().getGameState().getLayersManager().getDebugLayer().notify("auto saved...");
			_autoSaveTimer.reset();
		}
		
		std::stringstream s;
		s << "[X: " << (int)Mouse::get().getOffsetPosition().getX() << "][Y: " << (int)Mouse::get().getOffsetPosition().getY() << "]";
		if (getSelected())
			s << "[Selected: " << getSelected()->getEntity().getId().getString() << "]";
		PlayerPeriod* playerPeriod = static_cast<PlayerPeriod*>(getEntity().getManager().getEntity(PLAYER_ID)->get(ComponentsIds::PLAYER_PERIOD));
		if (playerPeriod && playerPeriod->getPeriod() != Period::NONE)
			s << "[Period: " << (playerPeriod->getPeriod() == Period::PAST ? "Past" : "Present") << "]";
		getEntity().getManager().getGameState().getLayersManager().getDebugLayer().showInfo(s.str().c_str());
	}

	void GameStateEditor::save()
	{
		XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/game-states/save-test.xml", true, false));
		serializer.serialize("game-state", getEntity().getManager().getGameState());
		serializer.save();
		XmlSerializer serializer2(new FileStream("resources/game-states/save-test.xml", true, false));
		serializer2.serialize("game-state", getEntity().getManager().getGameState());
		serializer2.save();
	}

	void GameStateEditor::handleKey(Key::Enum key)
	{
		if (key == Key::Q)
		{
			if (getEntity().getManager().getFocusInputComponent() == this)
			{
				getEntity().getManager().remove(CURSOR_ENTITY_ID);
				getEntity().getManager().getGameState().getEntityTemplatesManager().previousTemplate();
				setCursor();
			}
		}
		else if (key == Key::E)
		{
			if (getEntity().getManager().getFocusInputComponent() == this)
			{
				getEntity().getManager().remove(CURSOR_ENTITY_ID);
				getEntity().getManager().getGameState().getEntityTemplatesManager().nextTemplate();
				setCursor();
			}
		}
		else if (key == Key::F1)
		{
			clearCursor();
			const char* previewFile = "resources/game-states/save-test-preview.xml";
			XmlSerializer serializer(new FileStream(previewFile, true, false));
			serializer.serialize("game-state", getEntity().getManager().getGameState());
			serializer.save();
			GameStateManager::get().syncLoadAndShow(previewFile);
			GameState& gameState = GameStateManager::get().getStateById(Hash(previewFile));
			Entity* entity = new Entity(ComponentsIds::GAME_STATE_EDITOR);
			GameStateEditorPreview* preview = new GameStateEditorPreview();
			entity->add(preview);
			DebugManager* debugManager = new DebugManager();
			entity->add(debugManager);
			gameState.getEntitiesManager().add(entity);
		}
		else if (key == Key::F2)
		{
			if (getEntity().getManager().getFocusInputComponent() == this)
			{
				clearCursor();
			}
			else
			{
				setCursor();
				getEntity().getManager().setFocusInputComponent(this);
			}

		}
		else if (key == Key::F5)
		{
			save();
		}
		else if (key == Key::DEL)
		{
			deleteEntity();
		}
		else if (key == Key::C)
		{
			if (getSelected())
			{
				
				clearCursor();
				Hash id = getSelected()->getEntity().getId();
				_idPrefix = id.getString();
				Entity* newEntity = getEntity().getManager().getGameState().getEntitiesManager().getEntity(id)->clone();
				addEntity(newEntity, CURSOR_ENTITY_ID, true);
				getEntity().getManager().setFocusInputComponent(this);
			}
		}
		else if (key == Key::BACKSPACE)
		{
			PlayerPeriod* playerPeriod = static_cast<PlayerPeriod*>(getEntity().getManager().getEntity(PLAYER_ID)->get(ComponentsIds::PLAYER_PERIOD));
			if (playerPeriod)
				playerPeriod->changePeriod(Period::NONE);
		}
		else if (key == Key::Z)
		{
			if (_undo <= 0)
				return;
			--_undo;
			std::string undoFile = Utils::format("resources/game-states/undo/%d.xml", _undo);
			GameStateManager::get().syncUnloadCurrent();
			loadEditor(undoFile.c_str(), _undo);
		}
		else if (key == Key::TAB)
		{
			if (!getSelected())
			{
				return;
			}
			Hash id("component-editor");
			if (getEntity().getManager().getEntity(id))
				return;
			
			ComponentList components;
			ActivationSensor* activationSensor = static_cast<ActivationSensor*>(getSelected()->getEntity().get(ComponentsIds::ACTIVATION_SENSOR));
			if (activationSensor)
			{
				components.push_back(new ActivationSensorEditor(*activationSensor));
			}
			Button* button = static_cast<Button*>(getSelected()->getEntity().get(ComponentsIds::BUTTON));
			if (button)
			{
				components.push_back(new ButtonEditor(*button));
			}
			Laser* laser = static_cast<Laser*>(getSelected()->getEntity().get(ComponentsIds::LASER));
			if (laser)
			{
				components.push_back(new LaserEditor(*laser));
			}
			Light* light = static_cast<Light*>(getSelected()->getEntity().get(ComponentsIds::LIGHT));
			if (light)
			{
				components.push_back(new LightEditor(*light));
			}
			Door* door = static_cast<Door*>(getSelected()->getEntity().get(ComponentsIds::DOOR));
			if (door)
			{
				components.push_back(new DoorEditor(*door));
			}
			TemporalPeriod* period = static_cast<TemporalPeriod*>(getSelected()->getEntity().get(ComponentsIds::TEMPORAL_PERIOD));
			if (period)
			{
				components.push_back(new TemporalPeriodEditor(*period));
			}
			Patrol* patrol = static_cast<Patrol*>(getSelected()->getEntity().get(ComponentsIds::PATROL));
			if (patrol)
			{
				components.push_back(new PatrolEditor(*patrol));
			}
			if (components.size() != 0)
			{
				Entity* entity = new Entity(id);
				entity->setBypassSave(true);
				for (ComponentIterator i = components.begin(); i != components.end(); ++i)
				{
					entity->add(*i);
				}
				getEntity().getManager().add(entity);
			}
		}
	}

	void GameStateEditor::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().setBypassSave(true);
			getEntity().getManager().addInputComponent(this);

			HashEntityMap& entities = getEntity().getManager().getEntities();
			for (HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				addEditableToEntity(entity, *this);
			}
			setEditorMode();
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			float framePeriod = getFloatParam(message.getParam());
			update(framePeriod);
		}
		else if (message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			handleKey(key);
		}
		else if (message.getID() == MessageID::MOUSE_UP)
		{
			MouseParams& params = getMouseParams(message.getParam());
			if (params.getButton() == MouseButton::LEFT)
				leftClick(params);
		}
	}

	void GameStateEditor::setEditorMode()
	{
		HashList ids;
		ids.push_back(ComponentsIds::GAME_STATE_EDITOR);
		ids.push_back(ComponentsIds::EDITABLE);
		ids.push_back(ComponentsIds::CONTROL);
		getEntity().getManager().getGameState().setUpdateFilter(ids);
		getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(false);

	}

	void GameStateEditor::leftClick(const MouseParams& params)
	{
		addUndo();
		int idIndex = 0;
		std::string key(_idPrefix);
		int i = key.find_first_of("0123456789");
		if (i != std::string::npos)
			key.erase(i, key.size());
		Hash id;
		do
		{
			std::string idString = Utils::format("%s%d", key.c_str(), idIndex);
			id = Hash(idString.c_str());
			idIndex++;
		} while (getEntity().getManager().getEntity(id));
		

		Entity* newEntity = getEntity().getManager().getGameState().getEntitiesManager().getEntity(CURSOR_ENTITY_ID)->clone();
		addEntity(newEntity, id);
		addEditableToEntity(*newEntity, *this);
		
		PlayerPeriod* playerPeriod = static_cast<PlayerPeriod*>(getEntity().getManager().getEntity(PLAYER_ID)->get(ComponentsIds::PLAYER_PERIOD));
		if (playerPeriod && playerPeriod->getPeriod() != Period::NONE)
		{
			TemporalPeriod* temporalPeriod = static_cast<TemporalPeriod*>(newEntity->get(ComponentsIds::TEMPORAL_PERIOD));
			if (temporalPeriod)
			{
				temporalPeriod->setPeriod(playerPeriod->getPeriod());
			}
			else
			{
				temporalPeriod = new TemporalPeriod(playerPeriod->getPeriod());
				newEntity->add(temporalPeriod);
			}
		}
	}

	void GameStateEditor::deleteEntity()
	{
		if (getSelected())
		{
			addUndo();
			getEntity().getManager().remove(getSelected()->getEntity().getId());
			setSelected(0);
		}
	}

	void GameStateEditor::clearCursor()
	{
		if (getEntity().getManager().getFocusInputComponent() == this)
		{
			getEntity().getManager().remove(CURSOR_ENTITY_ID);
			getEntity().getManager().clearFocusInputComponent();
		}
	}

	void GameStateEditor::setCursor()
	{
		Entity* newEntity = getEntity().getManager().getGameState().getEntityTemplatesManager().cloneCurrent();
		_idPrefix = newEntity->getId().getString();
		addEntity(newEntity, CURSOR_ENTITY_ID, true);
		DebugManager& debugManager = *static_cast<DebugManager*>(getEntity().get(ComponentsIds::DEBUG_MANAGER));
		debugManager.addDebugRendererToEntity(*newEntity);
	}

	void GameStateEditor::addEntity(Entity* newEntity, Hash id, bool bypassSave)
	{
		newEntity->setId(id);
		newEntity->setBypassSave(bypassSave);
		getEntity().getManager().getGameState().getEntitiesManager().add(newEntity);
	
		Vector position = Mouse::get().getOffsetPosition();
		OBB shape = Editable::getShape(*newEntity);
		float tileSize = getEntity().getManager().getGameState().getGrid().getTileSize();
		position.setX(ToolsUtils::snap(position.getX(), tileSize / 4.0f, 8.0f, shape.getRadiusX()));
		position.setY(ToolsUtils::snap(position.getY(), tileSize / 4.0f, 8.0f, shape.getRadiusY()));

		newEntity->get(TRANSFORM)->handleMessage(Message(MessageID::SET_POSITION, &position));
	}

	void GameStateEditor::moveCamera(const Vector& direction)
	{
		if (getEntity().getManager().getFocusInputComponent() && getEntity().getManager().getFocusInputComponent() != this)
			return;
		Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
		camera.translate(direction);
	}

	void GameStateEditor::addUndo()
	{
		XmlSerializer serializer(new FileStream(Utils::format("resources/game-states/undo/%d.xml", _undo).c_str(), true, false));
		++_undo;
		serializer.serialize("game-state", getEntity().getManager().getGameState());
		serializer.save();
	}

	void GameStateEditor::loadEditor(const char* path, int undo)
	{
		
		if (!path)
			path = GameStateManager::get().getCurrentStateId().getString();
		GameStateManager::get().syncLoadAndShow(path);
		GameState& gameState = GameStateManager::get().getStateById(Hash(path));
		Entity* entity = new Entity(ComponentsIds::GAME_STATE_EDITOR);
		GameStateEditor* editor = new GameStateEditor(undo);
		entity->add(editor);
		DebugManager* debugManager = new DebugManager(true);
		entity->add(debugManager);
		gameState.getEntitiesManager().add(entity);
	}

	void GameStateEditorPreview::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if (key == Key::F1)
			{
				GameStateManager::get().syncUnloadCurrent();
			}
		}
	}
}
