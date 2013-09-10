#include "Hash.h"
#include "EntitySystem.h"
#include "Keyboard.h"
#include "Animation.h"
#include "ActionController.h"
#include "DynamicBody.h"
#include "Transform.h"
#include "Text.h"
#include <sstream>

// Movement simulator
namespace Temporal
{
	static const Hash PLAYER_ENTITY = Hash("ENT_PLAYER");

	namespace MovementSimulatorTest
	{
		enum Enum
		{
			SPEED,
			ACCELERATION,
			JUMP,
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
				else if(_test == MovementSimulatorTest::JUMP)
				{
					name = "Jump";
					value = &ActionController::JUMP_FORCE_PER_SECOND;
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
				stream << name << ": " << *value << "\n";
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

	class MyGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState)
		{
			if(id == Hash("resources/game-states/entities.xml"))
			{
			
				Entity* entity = new Entity();
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