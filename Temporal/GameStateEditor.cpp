#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Layer.h"
#include "Camera.h"
#include "Graphics.h"
#include "Control.h"
#include "MessageUtils.h"
#include "Utils.h"
#include "Game.h"

namespace Temporal
{
	const Hash GameStateEditor::TYPE = Hash("ENT_GAME_STATE_EDITOR");

	void GameStateEditor::handleMessage(Message& message)
	{
		bool editorMode = getEntity().getManager().getGameState().getUpdateFilter().size() != 0;
		if(message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			HashEntityMap& entities = getEntity().getManager().getEntities();
			Hash rendererComponentID = Hash("static-body");
			for(EntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				if(entity.get(rendererComponentID))
					entity.add(new Editable());
			}

			Vector size = Graphics::get().getLogicalView();
			Control* control = addControl(Hash("skeletonPanel"), AABB(size / 2.0f, size));
			control->setBackgroundColor(Color::Transparent);
			control->setHoverColor(Color::Transparent);

			control->setLeftMouseClickEvent(createAction1(GameStateEditor, const MouseParams&, leftClick));
			
		}
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			setEditorMode(true);
			Keyboard::get().add(this);

			XmlDeserializer deserializer(new FileStream("resources/game-states/templates.xml", false, false));
			deserializer.serialize("entity", _templates);
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
		}
		else if(message.getID() == MessageID::KEY_UP)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(key == Key::F2)
			{
				XmlSerializer serializer(new FileStream("../temporal/external/bin/resources/game-states/save-test.xml", true, false));
				serializer.serialize("game-state", getEntity().getManager().getGameState());
				serializer.save();
			}
			else if (key == Key::F1)
			{
				setEditorMode(!editorMode);
			}
		}
	}

	void GameStateEditor::setEditorMode(bool editorMode)
	{
		HashList ids;
		if (editorMode)
			ids.push_back(GameStateEditor::TYPE);
		getEntity().getManager().getGameState().setUpdateFilter(ids);
		getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(!editorMode);

	}

	void GameStateEditor::leftClick(const MouseParams& params)
	{
		Entity* newEntity = _templates.at(Hash("TEM_PATROL"))->clone();
		Vector position = params.getPosition();
		newEntity->get(Hash("transform"))->handleMessage(Message(MessageID::SET_POSITION, &position));
		int idIndex = 0;
		const char * idStringFormat = "ENT_PATROL%d";
		Hash id;
		do
		{
			std::string idString = Utils::format(idStringFormat, idIndex);
			id = Hash(idString.c_str());
			idIndex++;
		} while (getEntity().getManager().getEntity(id));
		newEntity->setId(id);
		getEntity().getManager().add2(newEntity);
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