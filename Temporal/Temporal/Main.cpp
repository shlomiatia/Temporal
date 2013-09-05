#include "Main.h"
#include "Game.h"
#include <SDL.h>

#include "EntitySystem.h"
#include "Keyboard.h"
#include "Serialization.h"
#include "GameState.h"
#include "MessageUtils.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "SaverLoader.h"
#include "Input.h"
#include "ActionController.h"
#include "Text.h"
#include <sstream>

namespace Temporal
{
	enum MovementSimulatorTest
	{
		SPEED,
		JUMP
	};

	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	class MovementSimulator : public Component
	{
	public:
		Hash getType() const { return TYPE; }

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				if(Keyboard::get().isKeyDown(Key::P))
					_test = SPEED;
				else if(Keyboard::get().isKeyDown(Key::J))
					_test = JUMP;
				const char* name = 0;
				float* value = 0;
				if(_test == SPEED)
				{
					name = "Speed";
					value = &ActionController::WALK_FORCE_PER_SECOND;
				}
				else if(_test == JUMP)
				{
					name = "Jump";
					value = &ActionController::JUMP_FORCE_PER_SECOND;
				}

				if(Keyboard::get().isKeyPressed(Key::PLUS))
				{
					(*value)++;
				}
				else if(Keyboard::get().isKeyPressed(Key::MINUS))
				{
					(*value)--;
				}

				std::ostringstream stream;
				stream << name << ": " << *value;
				this->raiseMessage(Message(MessageID::SET_TEXT, const_cast<char *>(stream.str().c_str())));
			}
		}
		Component* clone() const { return 0; }

		MovementSimulator() : _test(SPEED) {}

	private:
		static const Hash TYPE;
		MovementSimulatorTest _test;
	};

	const Hash MovementSimulator::TYPE = Hash("movement-simulator");

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
		void onUpdate(float framePeriod, GameState& gameState)
		{
			if(Keyboard::get().isKeyDown(Key::Q))
			{
				//const AABB& bounds = *static_cast<AABB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
				//getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
				//getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
			}
		}
		virtual void onLoaded(Hash id, GameState& gameState)
		{
			if(id == Hash("resources/game-states/loading.xml"))
			{
				Entity* entity = new Entity();
				entity->add(new GameStateLoaderComponent());
				gameState.getEntitiesManager().add(Hash("ENT_TEST"), entity);
			}
			else
			{
				Entity* entity = new Entity();
				entity->add(new GameSaverLoader());
				gameState.getEntitiesManager().add(Hash("ENT_SAVER_LOADER"), entity);
				entity = new Entity();
				entity->add(new MovementSimulator());
				Text* text = new Text("c:/windows/fonts/Arial.ttf", 12);
				entity->add(text);
				gameState.getEntitiesManager().add(Hash("ENT_MOVEMENT_SIMULATOR"), entity);
			}
		}
	};
}

using namespace Temporal;

int main(int argc, char* argv[])
{
	{
		std::string s;
	}
	GameStateManager::get().setListener(new MyGameStateListener());
	Game::get().run("resources/game-states/entities.xml");
//	_CrtDumpMemoryLeaks();
	return 0;
}

