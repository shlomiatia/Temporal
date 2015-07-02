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
		handleGridFixtureMessage(message, *this, *_fixture);
		if(message.getID() == MessageID::ENTITY_INIT)
		{
			getEntity().getManager().getGameState().getLayersManager().addSprite(LayerType::STATIC, this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().getGameState().getLayersManager().removeSprite(LayerType::STATIC, this);
		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			if (_fixture->getCategory() != 0)
				Graphics::get().getSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f));
			else
				Graphics::get().getSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f, 0.5f));
		}
	}
}