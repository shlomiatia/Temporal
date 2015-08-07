#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Camera.h"
#include "Graphics.h"
#include "MessageUtils.h"
#include "Utils.h"
#include "Game.h"
#include "Mouse.h"

namespace Temporal
{
	const Hash GameStateEditor::TYPE = Hash("game-state-editor");
	const Hash GameStateEditorPreview::TYPE = Hash("game-state-editor-preview");
	const Hash CURSOR_ENTITY_ID("ENT_CURSOR");
	const HashList TRANSLATION_ONLY_EDITABLE_FILTER({ Hash("dynamic-body"), Hash("camera-control"), Hash("light") });
	const HashList EDITABLE_FILTER({ Hash("static-body"), Hash("renderer") });
	const Hash TRANSFORM = Hash("transform");

	void addEditableToEntity(Entity& entity)
	{
		for (HashIterator i = TRANSLATION_ONLY_EDITABLE_FILTER.begin(); i != TRANSLATION_ONLY_EDITABLE_FILTER.end(); ++i)
		{
			if (entity.get(*i))
			{
				entity.add(new Editable(true));
				return;
			}
				
		}
		for (HashIterator i = EDITABLE_FILTER.begin(); i != EDITABLE_FILTER.end(); ++i)
		{
			if (entity.get(*i))
			{
				entity.add(new Editable(false));
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
			Vector position = Mouse::get().getOffsetPosition();
			getEntity().getManager().sendMessageToEntity(CURSOR_ENTITY_ID, Message(MessageID::SET_POSITION, &position));
		}
	}

	void GameStateEditor::handleKey(Key::Enum key)
	{
		if (key == Key::Q)
		{
			if (getEntity().getManager().getFocusInputComponent() == this)
			{
				getEntity().getManager().remove(CURSOR_ENTITY_ID);
				getEntity().getManager().getGameState().getEntityTemplatesManager().previousTemplate();
				Entity* newEntity = getEntity().getManager().getGameState().getEntityTemplatesManager().cloneCurrent();
				addCursor(newEntity);
			}
		}
		else if (key == Key::E)
		{
			if (getEntity().getManager().getFocusInputComponent() == this)
			{
				getEntity().getManager().remove(CURSOR_ENTITY_ID);
				getEntity().getManager().getGameState().getEntityTemplatesManager().nextTemplate();
				Entity* newEntity = getEntity().getManager().getGameState().getEntityTemplatesManager().cloneCurrent();
				addCursor(newEntity);
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
			Entity* entity = new Entity(Hash("ENT_GAME_STATE_EDITOR_PREVIEW"));
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
				Entity* newEntity = getEntity().getManager().getGameState().getEntityTemplatesManager().cloneCurrent();
				addCursor(newEntity);
				getEntity().getManager().setFocusInputComponent(this);
			}

		}
		else if (key == Key::F5)
		{
			XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/game-states/save-test.xml", true, false));
			serializer.serialize("game-state", getEntity().getManager().getGameState());
			serializer.save();
		}
		else if (key == Key::DEL)
		{
			if (Editable::getSelected())
			{
				getEntity().getManager().remove(Editable::getSelected()->getEntity().getId());
				Editable::clearSelected();
			}
		}
		else if (key == Key::C)
		{
			if (Editable::getSelected())
			{
				clearCursor();
				Entity* newEntity = getEntity().getManager().getGameState().getEntitiesManager().getEntity(Editable::getSelected()->getEntity().getId())->clone();
				addEntity(newEntity, CURSOR_ENTITY_ID, true);
				getEntity().getManager().setFocusInputComponent(this);
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
				addEditableToEntity(entity);
			}
			setEditorMode();
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::LEVEL_INIT)
		{
			
			/*HashEntityMap& entities = getEntity().getManager().getEntities();
			for (HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				addEditableToEntity(entity);
			}
			setEditorMode();*/
			
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
		ids.push_back(GameStateEditor::TYPE);
		ids.push_back(Editable::TYPE);
		getEntity().getManager().getGameState().setUpdateFilter(ids);
		getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(false);

	}

	void GameStateEditor::leftClick(const MouseParams& params)
	{
		int idIndex = 0;
		const char* key = getEntity().getManager().getGameState().getEntityTemplatesManager().getCurrentTemplateId().getString();
		Hash id;
		do
		{
			std::string idString = Utils::format("%s%d", key, idIndex);
			id = Hash(idString.c_str());
			idIndex++;
		} while (getEntity().getManager().getEntity(id));
		

		Entity* newEntity = getEntity().getManager().getGameState().getEntitiesManager().getEntity(CURSOR_ENTITY_ID)->clone();
		Vector position = params.getPosition();

		addEntity(newEntity, id);
	}

	void GameStateEditor::clearCursor()
	{
		if (getEntity().getManager().getFocusInputComponent() == this)
		{
			getEntity().getManager().remove(CURSOR_ENTITY_ID);
			getEntity().getManager().clearFocusInputComponent();
		}
	}

	void GameStateEditor::addCursor(Entity* newEntity)
	{
		addEntity(newEntity, CURSOR_ENTITY_ID, true);
		addEditableToEntity(*newEntity);
	}

	void GameStateEditor::addEntity(Entity* newEntity, Hash id, bool bypassSave)
	{
		Vector position = Mouse::get().getOffsetPosition();
		newEntity->get(TRANSFORM)->handleMessage(Message(MessageID::SET_POSITION, &position));
		newEntity->setId(id);
		newEntity->setBypassSave(true);
		getEntity().getManager().getGameState().getEntitiesManager().add(newEntity);
	}

	void GameStateEditor::moveCamera(const Vector& direction)
	{
		Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
		camera.translate(direction);
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