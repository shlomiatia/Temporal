#include "StaticBody.h"
#include "Graphics.h"
#include "Fixture.h"
#include "Grid.h"

namespace Temporal
{
	StaticBody::StaticBody(Fixture* fixture) : _fixture(fixture)
	{
	}
	StaticBody::~StaticBody()
	{
		delete _fixture;
	}

	void StaticBody::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::ENTITY_CREATED)
		{
			_fixture->init(*this);
			Grid::get().add(_fixture);

		}
		else if(message.getID() == MessageID::DRAW_DEBUG)
		{
			Graphics::get().draw(_fixture->getGlobalShape(), Color(0.5f, 0.25f, 0.125f));
		}
		else if(message.getID() == MessageID::GET_SHAPE)
		{
			Shape* shape = const_cast<Shape*>(&_fixture->getGlobalShape());
			message.setParam(shape);
		}
	}
}