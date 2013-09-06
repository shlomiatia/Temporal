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
#include "Animation.h"
#include "DynamicBody.h"
#include <sstream>

namespace Temporal
{
	namespace MovementSimulatorTest
	{
		enum Enum
		{
			SPEED,
			ACCELERATION,
			WALK,
			JUMP,
			TURN,
			CLIFF,
			GRAVITY,
			FALL
		};
	}

	class MovementSimulator : public Component
	{
	public:
		Hash getType() const { return TYPE; }

		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				std::shared_ptr<AnimationSet> animationSet =  ResourceManager::get().getAnimationSet("resources/animations/pop.xml");
				if(Keyboard::get().isKeyDown(Key::P))
					_test = MovementSimulatorTest::SPEED;
				else if(Keyboard::get().isKeyDown(Key::C))
					_test = MovementSimulatorTest::ACCELERATION;
				else if(Keyboard::get().isKeyDown(Key::K))
					_test = MovementSimulatorTest::WALK;
				else if(Keyboard::get().isKeyDown(Key::J))
					_test = MovementSimulatorTest::JUMP;
				else if(Keyboard::get().isKeyDown(Key::T))
					_test = MovementSimulatorTest::TURN;
				else if(Keyboard::get().isKeyDown(Key::F))
					_test = MovementSimulatorTest::CLIFF;
				else if(Keyboard::get().isKeyDown(Key::G))
					_test = MovementSimulatorTest::GRAVITY;
				else if(Keyboard::get().isKeyDown(Key::L))
					_test = MovementSimulatorTest::FALL;
				const char* name = 0;
				float* value = 0;
				float temp = 0.0f;
				Hash animationId = Hash::INVALID;
				Animation* animation = 0;
				Sample* sample = 0;
				if(_test == MovementSimulatorTest::SPEED)
				{
					name = "Speed";
					value = &ActionController::MAX_WALK_FORCE_PER_SECOND;
				}
				else if(_test == MovementSimulatorTest::ACCELERATION)
				{
					name = "Acceleration";
					value = &ActionController::WALK_ACC_PER_SECOND;
				}
				else if(_test == MovementSimulatorTest::WALK)
				{
					name = "Walk";
					animationId = WALK;
				}
				else if(_test == MovementSimulatorTest::JUMP)
				{
					name = "Jump";
					value = &ActionController::JUMP_FORCE_PER_SECOND;
				}
				else if(_test == MovementSimulatorTest::TURN)
				{
					name = "Turn";
					animationId = TURN;
				}
				else if(_test == MovementSimulatorTest::CLIFF)
				{
					name = "Climb";
					animationId = CLIMB;
				}
				else if(_test == MovementSimulatorTest::GRAVITY)
				{
					name = "Gravity";
					temp = DynamicBody::GRAVITY.getY();
					value = &temp;
				}
				else if(_test == MovementSimulatorTest::FALL)
				{
					name = "Fall";
					temp = ActionController::FALL_ALLOW_JUMP_TIME * 1000.0f;
					value = &temp;
				}
				
				if(animationId != Hash::INVALID)
				{
					animation = &const_cast<Animation&>(animationSet->get(animationId));
					sample = animation->get(Hash::INVALID).get()[0];
					temp = sample->getDuration() * 1000.0f;
					value = &temp;
				}
				
				if(Keyboard::get().isKeyPressed(Key::PLUS))
				{
					(*value)++;
				}
				else if(Keyboard::get().isKeyPressed(Key::MINUS))
				{
					(*value)--;
				}

				if(animationId != Hash::INVALID)
				{
					sample->setDuration(temp/1000.0f);
					animation->init();
					if(animationId == CLIMB)
					{
						animation = &const_cast<Animation&>(animationSet->get(DESCEND));
						sample = animation->get(Hash::INVALID).get()[0];
						sample->setDuration(temp/1000.0f);
						animation->init();
					}
				}
				else if(_test == MovementSimulatorTest::GRAVITY)
				{
					DynamicBody::GRAVITY.setY(temp);
				}
				else if(_test == MovementSimulatorTest::FALL)
				{
					ActionController::FALL_ALLOW_JUMP_TIME = temp/1000.0f;
				}


				std::ostringstream stream;
				stream << name << ": " << *value;
				this->raiseMessage(Message(MessageID::SET_TEXT, const_cast<char *>(stream.str().c_str())));
			}
		}
		Component* clone() const { return 0; }

		MovementSimulator() : _test(MovementSimulatorTest::SPEED) {}

	private:
		static const Hash TYPE;
		static const Hash TURN;
		static const Hash WALK;
		static const Hash CLIMB;
		static const Hash DESCEND;
		MovementSimulatorTest::Enum _test;
	};

	const Hash MovementSimulator::TYPE = Hash("movement-simulator");
	const Hash MovementSimulator::TURN = Hash("POP_ANM_TURN");
	const Hash MovementSimulator::WALK = Hash("POP_ANM_WALK");
	const Hash MovementSimulator::CLIMB = Hash("POP_ANM_CLIMB");
	const Hash MovementSimulator::DESCEND = Hash("POP_ANM_DESCEND");

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

