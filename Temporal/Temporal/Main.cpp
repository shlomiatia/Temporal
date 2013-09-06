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
#include "Transform.h"
#include <sstream>

namespace Temporal
{
	namespace MovementSimulatorTest
	{
		enum Enum
		{
			SPEED,
			ACCELERATION,
			GRAVITY,
			FALL_TIME,
			JUMP_STOP_MODIFIER,
			WALK_JUMP_MODIFIER,
			WALK_ANIMATION,
			JUMP_ANIMATION,
			TURN_ANIMATION,
			CLIFF_ANIMATION,
			SIZE
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
				if(Keyboard::get().isKeyDown(Key::OPEN_BRACKET))
					_test = (MovementSimulatorTest::Enum)((MovementSimulatorTest::SIZE + _test - 1) % MovementSimulatorTest::SIZE);
				else if(Keyboard::get().isKeyDown(Key::CLOSE_BRACKET))
					_test = (MovementSimulatorTest::Enum)((MovementSimulatorTest::SIZE + _test + 1) % MovementSimulatorTest::SIZE);
				
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
				else if(_test == MovementSimulatorTest::GRAVITY)
				{
					name = "Gravity";
					temp = DynamicBody::GRAVITY.getY();
					value = &temp;
				}
				else if(_test == MovementSimulatorTest::FALL_TIME)
				{
					name = "Fall Time";
					temp = ActionController::FALL_ALLOW_JUMP_TIME * 1000.0f;
					value = &temp;
				}
				else if(_test == MovementSimulatorTest::JUMP_STOP_MODIFIER)
				{
					name = "Jump Stop Modifier";
					temp = ActionController::JUMP_STOP_MODIFIER * 1000.0f;
					value = &temp;
				}
				else if(_test == MovementSimulatorTest::WALK_JUMP_MODIFIER)
				{
					name = "Walk Jump Modifier";
					temp = ActionController::MAX_WALK_JUMP_MODIFIER* 1000.0f;
					value = &temp;
				}
				else if(_test == MovementSimulatorTest::WALK_ANIMATION)
				{
					name = "Walk Animation";
					animationId = WALK;
				}
				else if(_test == MovementSimulatorTest::JUMP_ANIMATION)
				{
					name = "Jump Animation";
					value = &ActionController::JUMP_FORCE_PER_SECOND;
				}
				else if(_test == MovementSimulatorTest::TURN_ANIMATION)
				{
					name = "Turn Animation";
					animationId = TURN;
				}
				else if(_test == MovementSimulatorTest::CLIFF_ANIMATION)
				{
					name = "Climb Animation";
					animationId = CLIMB;
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
				else if(_test == MovementSimulatorTest::FALL_TIME)
				{
					ActionController::FALL_ALLOW_JUMP_TIME = temp/1000.0f;
				}
				else if(_test == MovementSimulatorTest::JUMP_STOP_MODIFIER)
				{
					ActionController::JUMP_STOP_MODIFIER = temp/1000.0f;
				}
				else if(_test == MovementSimulatorTest::WALK_JUMP_MODIFIER)
				{
					ActionController::MAX_WALK_JUMP_MODIFIER = temp/1000.0f;
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
				entity->add(new Transform(Vector(0.0f, 100.0f)));
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

