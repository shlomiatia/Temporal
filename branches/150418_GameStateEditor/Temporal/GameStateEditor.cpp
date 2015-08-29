#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Layer.h"
#include "Camera.h"
#include "Graphics.h"
#include "MessageUtils.h"
#include "Utils.h"
#include "Game.h"
#include "Mouse.h"

namespace Temporal
{
	const Hash GameStateEditor::TYPE = Hash("ENT_GAME_STATE_EDITOR");
	const Hash CUSROR_ENTITY_ID("ENT_CURSOR");
	const Hash EDITABLE_FILTER("static-body");

	void GameStateEditor::handleMessage(Message& message)
	{
		bool editorMode = getEntity().getManager().getGameState().getUpdateFilter().size() != 0;
		if (message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			HashEntityMap& entities = getEntity().getManager().getEntities();
			for(HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				if(entity.get(EDITABLE_FILTER))
					entity.add(new Editable());
			}
		}
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			setEditorMode(true);
			getEntity().getManager().addInputComponent(this);

			XmlDeserializer deserializer(new FileStream("resources/game-states/templates.xml", false, false));
			deserializer.serialize("entity", _templates);
			_templateIterator = _templates.begin();
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(Keyboard::get().getKey(Key::W))
			{
				moveCamera(Vector(0.0f, 1.0f));
			}
			if(Keyboard::get().getKey(Key::A))
			{
				moveCamera(Vector(-1.0f, 0.0f));
			}
			if(Keyboard::get().getKey(Key::S))
			{
				moveCamera(Vector(0.0f, -1.0f));
			}
			if(Keyboard::get().getKey(Key::D))
			{
				moveCamera(Vector(1.0f, 0.0f));
			}
			if (getEntity().getManager().getFocusInputComponent()) 
			{
				Vector position = Mouse::get().getOffsetPosition();
				getEntity().getManager().sendMessageToEntity(CUSROR_ENTITY_ID, Message(MessageID::SET_POSITION, &position));
			}
		}
		else if(message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if (key == Key::Q)
			{
				if (getEntity().getManager().getFocusInputComponent())
				{
					if (_templateIterator == _templates.begin())
					{
						_templateIterator = _templates.end();
					}
					--_templateIterator;
					getEntity().getManager().remove(CUSROR_ENTITY_ID);
					Vector position = Mouse::get().getOffsetPosition();
					cloneEntityFromTemplate(CUSROR_ENTITY_ID, position);
				}
			}
			else if (key == Key::E)
			{
				if (getEntity().getManager().getFocusInputComponent())
				{
					++_templateIterator;
					if (_templateIterator == _templates.end())
						_templateIterator = _templates.begin();
					getEntity().getManager().remove(CUSROR_ENTITY_ID);
					Vector position = Mouse::get().getOffsetPosition();
					cloneEntityFromTemplate(CUSROR_ENTITY_ID, position);
				}
			}
			else if (key == Key::F1)
			{
				setEditorMode(!editorMode);
			}
			else if (key == Key::F2)
			{
				if (getEntity().getManager().getFocusInputComponent())
				{ 
					getEntity().getManager().remove(CUSROR_ENTITY_ID);
					getEntity().getManager().clearFocusInputComponent();
				}
				else
				{
					Vector position = Mouse::get().getOffsetPosition();
					cloneEntityFromTemplate(CUSROR_ENTITY_ID, position);
					getEntity().getManager().setFocusInputComponent(this);
				}
					
			}
			else if (key == Key::F5)
			{
				XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/game-states/save-test.xml", true, false));
				serializer.serialize("game-state", getEntity().getManager().getGameState());
				serializer.save();
			}
			
		}
		else if (message.getID() == MessageID::MOUSE_UP)
		{
			MouseParams& params = getMouseParams(message.getParam());
			if (params.getButton() == MouseButton::LEFT)
				leftClick(params);
		}
	}

	void GameStateEditor::setEditorMode(bool editorMode)
	{
		HashList ids;
		if (editorMode) 
		{
			ids.push_back(GameStateEditor::TYPE);
			ids.push_back(Editable::TYPE);
		}
			
		getEntity().getManager().getGameState().setUpdateFilter(ids);
		getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(!editorMode);

	}

	void GameStateEditor::leftClick(const MouseParams& params)
	{
		int idIndex = 0;
		const char* key = (*_templateIterator).first.getString();
		Hash id;
		do
		{
			std::string idString = Utils::format("%s%d", key, idIndex);
			id = Hash(idString.c_str());
			idIndex++;
		} while (getEntity().getManager().getEntity(id));
		Vector position = params.getPosition();
		cloneEntityFromTemplate(id, position);
	}

	void GameStateEditor::cloneEntityFromTemplate(Hash id, Vector& position)
	{
		Entity* newEntity = (*_templateIterator).second->clone();
		if (newEntity->get(EDITABLE_FILTER))
			newEntity->add(new Editable());
		newEntity->get(Hash("transform"))->handleMessage(Message(MessageID::SET_POSITION, &position));
		newEntity->setId(id);
		getEntity().getManager().add(newEntity);
	}

	void GameStateEditor::moveCamera(const Vector& direction)
	{
		Camera& camera = getEntity().getManager().getGameState().getLayersManager().getCamera();
		camera.setBottomLeft(camera.getBottomLeft() + direction * 5.0f);
	}

	void GSEGameStateListener::onLoaded(Hash id, GameState& gameState)
	{
		if(id == Hash("resources/game-states/entities.xml"))
		{
			Entity* entity = new Entity(GameStateEditor::TYPE);
			entity->add(new GameStateEditor());
			gameState.getEntitiesManager().add(entity);
		}
	}
}