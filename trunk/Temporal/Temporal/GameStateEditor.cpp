#include "GameStateEditor.h"
#include "Editable.h"

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