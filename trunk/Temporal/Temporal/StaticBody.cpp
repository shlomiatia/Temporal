#include "StaticBody.h"
#include "Graphics.h"
#include "Fixture.h"
#include "Grid.h"
#include "MessageUtils.h"
#include "CollisionFilter.h"
#include "Layer.h"

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
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().getGameState().getGrid().remove(_fixture);
			getEntity().getManager().getGameState().getLayersManager().removeSprite(LayerType::STATIC, this);
		}
		else if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().getGameState().getLayersManager().addSprite(LayerType::STATIC, this);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			if (_fixture->getFilter().getCategory() != 0)
				Graphics::get().getSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f));
			else
				Graphics::get().getSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f, 0.5f));
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			const OBB* shape = &_fixture->getGlobalShape();
			message.setParam(const_cast<OBB*>(shape));
		}
		else if(message.getID() == MessageID::SET_POSITION)
		{
			getEntity().getManager().getGameState().getGrid().update(_fixture);
		}
	}
}