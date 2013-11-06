#include "Hash.h"
#include "GameState.h"
#include "Keyboard.h"
#include "EntitySystem.h"
#include "Transform.h"
#include "Timer.h"
#include "MessageUtils.h"

// Game state loader/Game saver loader/navigation/merge to temporal echoes
namespace Temporal
{
	class GameStateLoaderComponent : public Component
	{
	public:
		Hash getType() const { return TYPE; }
		virtual Component* clone() const { return 0; }
		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				float period = getFloatParam(message.getParam());
				_timer.update(period);
				if(_timer.getElapsedTime() > 3.0f)
				{
					if(!_loader.isStarted())
					{
						_loader.add("resources/game-states/entities.xml");
						GameStateManager::get().load(&_loader);
					}
				}
				if(_loader.isFinished())
				{
					StringCollection files;
					files.push_back(std::string("resources/game-states/loading.xml"));
					GameStateManager::get().unload(files);
					GameStateManager::get().show("resources/game-states/entities.xml");
				}
			}
		}
		static const Hash TYPE;
	private:

		Timer _timer;
		GameStateLoader _loader;
	};

	const Hash GameStateLoaderComponent::TYPE = Hash("game-state-loader");

	class MyGameStateListener : public GameStateListener
	{
	public:
		void onUpdate(float framePeriod)
		{
			/*if(Keyboard::get().isStartPressing(Key::Q))
			{
				const AABB& bounds = *static_cast<AABB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
				getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
				getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
			}*/
		}
		virtual void onLoaded(Hash id, GameState& gameState)
		{
			if(id == Hash("resources/game-states/loading.xml"))
			{
				/*Entity* entity = new Entity();
				entity->add(new GameSaverLoader());
				gameState.getEntitiesManager().add(Hash("ENT_SAVER_LOADER"), entity);*/
				Entity* entity = new Entity();
				entity->add(new GameStateLoaderComponent());
				gameState.getEntitiesManager().add(Hash("ENT_TEST"), entity);
			}
		}
	};
}