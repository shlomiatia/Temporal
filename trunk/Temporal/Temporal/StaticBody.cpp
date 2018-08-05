#include "StaticBody.h"
#include "Graphics.h"
#include "Fixture.h"
#include "Grid.h"
#include "MessageUtils.h"
#include "CollisionFilter.h"
#include "LayersManager.h"

namespace Temporal
{
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
				Graphics::get().getLinesSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f));
			else
				Graphics::get().getLinesSpriteBatch().add(_fixture->getGlobalShape(), Color(0.05f, 0.05f, 0.05f, 0.5f));
		}
	}
}