#include "StaticBody.h"
#include "Graphics.h"
#include "Fixture.h"
#include "Grid.h"
#include "MessageUtils.h"
#include "CollisionFilter.h"

namespace Temporal
{
	const Hash StaticBody::TYPE = Hash("static-body");

	StaticBody::~StaticBody()
	{
		delete _fixture;
	}

	Component* StaticBody::clone() const { return new StaticBody(_fixture->clone()); }

	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_POST_INIT)
		{
			_fixture->init(*this);
			getEntity().getManager().getGameState().getGrid().add(_fixture);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(0.5f, 0.25f, 0.125f));
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			OBB* shape = &_fixture->getGlobalShape();
			message.setParam(shape);
		}
		else if(message.getID() == MessageID::SET_POSITION)
		{
			getEntity().getManager().getGameState().getGrid().update(_fixture);
		}
	}
}