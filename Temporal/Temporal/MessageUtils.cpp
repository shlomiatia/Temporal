#include "MessageUtils.h"
#include "EntitySystem.h"
#include "Vector.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	const Vector& getPosition(const Component& component)
	{
		void* result = component.raiseMessage(Message(MessageID::GET_POSITION));
		if(result)
			return *static_cast<Vector*>(result);
		else
			return Vector::Zero;
	}

	const Vector& getPosition(const Entity& entity)
	{
		void* result = entity.handleMessage(Message(MessageID::GET_POSITION));
		if (result)
			return *static_cast<Vector*>(result);
		else
			return Vector::Zero;
	}

	Side::Enum getOrientation(const Component& component)
	{
		return *static_cast<Side::Enum*>(component.raiseMessage(Message(MessageID::GET_ORIENTATION)));
	}

	const OBB& getShape(const Component& component)
	{
		return *static_cast<OBB*>(component.raiseMessage(Message(MessageID::GET_SHAPE)));
	}

	Hash getHashParam(void* data)
	{
		return *static_cast<Hash*>(data);
	}

	const Vector& getVectorParam(void* data)
	{
		return *static_cast<const Vector*>(data);
	}

	void sendDirectionAction(const Component& component, Side::Enum direction)
	{
		Side::Enum orientation = getOrientation(component);
		if(direction == orientation)
			component.raiseMessage(Message(MessageID::ACTION_FORWARD));
		else
			component.raiseMessage(Message(MessageID::ACTION_BACKWARD));
	}

	void handleFixtureMessage(Message& message, Component& component, Fixture& fixture)
	{
		if (message.getID() == MessageID::GET_SHAPE)
		{
			if (!message.getParam())
			{
				const OBB* shape = &fixture.getGlobalShape();
				message.setParam(const_cast<OBB*>(shape));
			}
			
		}
		else if (message.getID() == MessageID::GET_FIXTURE)
		{
			message.setParam(&fixture);
		}
		else if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			fixture.init(component);
		}
		else if (message.getID() == MessageID::UPDATE)
		{
			fixture.update();
		}
	}

	void handleNonGridFixtureMessage(Message& message, Component& component, Fixture& fixture)
	{
		handleFixtureMessage(message, component, fixture);
		if (message.getID() == MessageID::SET_POSITION || message.getID() == MessageID::POST_LOAD)
		{
			fixture.update();
		}
	}

	void handleGridFixtureMessage(Message& message, Component& component, Fixture& fixture)
	{
		handleFixtureMessage(message, component, fixture);
		if (message.getID() == MessageID::ENTITY_POST_INIT)
		{
			component.getEntity().getManager().getGameState().getGrid().add(&fixture);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			component.getEntity().getManager().getGameState().getGrid().remove(&fixture);
		}
		else if (message.getID() == MessageID::SET_POSITION || message.getID() == MessageID::POST_LOAD)
		{
			component.getEntity().getManager().getGameState().getGrid().update(&fixture);
		}
	}
}