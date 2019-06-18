#include "PlayerPeriod.h"
#include "TemporalPeriod.h"
#include "MessageUtils.h"
#include "Color.h"
#include "Grid.h"
#include "Fixture.h"
#include "Transform.h"
#include "PhysicsEnums.h"

namespace Temporal 
{
	static const int MASK = CollisionCategory::OBSTACLE | CollisionCategory::CHARACTER;

	const ColorList PlayerPeriod::COLORS = {
		Color(0.6392156862745098f, 0.2862745098039216f, 0.6431372549019608f),
		Color(0.2470588235294118f, 0.2823529411764706f, 0.8f),
		Color(0.1333333333333333f, 0.6941176470588235f, 0.2980392156862745f),
		Color(1.0f, 0.4980392156862745f, 0.1529411764705882f),
		Color(1.0f, 0.6823529411764706f, 0.7882352941176471f),
		Color(0.7254901960784314f, 0.4784313725490196f, 0.3411764705882353f)
	};

	const Color& PlayerPeriod::getNextColor()
	{
		if (_colorIterator == COLORS.end())
			abort();
		const Color& color = *_colorIterator;
		++_colorIterator;
		return color;
	}

	void PlayerPeriod::changePeriod(Period::Enum period)
	{
		OBB shape = getShape(*this);
		shape.setRadius(shape.getRadius() - 1.0f);
		if (!getEntity().getManager().getGameState().getGrid().iterateTiles(shape, MASK, period))
		{
			void* result = raiseMessage(Message(MessageID::GET_DRAGGABLE));
			if (result)
			{
				Hash draggableId = getHashParam(result);
				Entity& draggable = *getEntity().getManager().getEntity(draggableId);
				TemporalPeriod& temporalPeriod = *static_cast<TemporalPeriod*>(draggable.get(ComponentsIds::TEMPORAL_PERIOD));
				if (!iterateTiles(draggable, MASK, period, temporalPeriod.getFutureSelfId()))
					return;
				temporalPeriod.setPeriod(period);
			}

			_period = period;
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::TEMPORAL_PERIOD_CHANGED, &_period));

		}
	}

	void PlayerPeriod::handleMessage(Message& message)
	{
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().addInputComponent(this);
			raiseMessage(Message(MessageID::SET_COLLISION_GROUP, &_period));
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::GAME_STATE_READY)
		{
			getEntity().getManager().sendMessageToAllEntities(Message(MessageID::TEMPORAL_PERIOD_CHANGED, &_period));
		}
		else if (message.getID() == MessageID::ACTION_TEMPORAL_TRAVEL)
		{
			Period::Enum period = _period == Period::PRESENT ? Period::PAST : Period::PRESENT;
			changePeriod(period);
		}
	}
}