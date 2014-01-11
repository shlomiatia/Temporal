#include "GameStateEditor.h"
#include "Editable.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Grid.h"
#include "Layer.h"

namespace Temporal
{
	void GameStateEditor::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			Keyboard::get().add(this);
		}
		else if(message.getID() == MessageID::ENTITY_PRE_INIT)
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