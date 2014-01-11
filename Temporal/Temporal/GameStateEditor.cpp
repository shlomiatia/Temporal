#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Layer.h"
#include "Camera.h"

namespace Temporal
{
	void GameStateEditor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_PRE_INIT)
		{
			EntityCollection& entities = getEntity().getManager().getEntities();
			Hash rendererComponentID = Hash("static-body");
			for(EntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				if(entity.get(rendererComponentID))
					entity.add(new Editable());
			}
		}
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().getGameState().getLayersManager().getCamera().setFollowPlayer(false);
			Keyboard::get().add(this);
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
		}
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
			Entity* entity = new Entity(Hash("ENT_GAME_STATE_EDITOR"));
			entity->add(new GameStateEditor());
			gameState.getEntitiesManager().add(entity);
		}
	}
}