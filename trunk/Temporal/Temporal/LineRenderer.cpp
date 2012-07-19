#include "LineRenderer.h"
#include "Graphics.h"
#include "Serialization.h"
#include "BaseUtils.h"
#include "Shapes.h"

namespace Temporal
{
	static const NumericPairSerializer TARGET_SERIALIZER("LRE_SER_TAR");

	void LineRenderer::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::SET_COLOR)
		{
			_color = *static_cast<Color*>(message.getParam());
		}
		else if(message.getID() == MessageID::SET_TARGET)
		{
			_target = *static_cast<Point*>(message.getParam());
		}
		else if(message.getID() == MessageID::DRAW)
		{
			VisualLayer::Enum layer = *static_cast<VisualLayer::Enum*>(message.getParam());
			if(_layer == layer)
				draw();			
		}
		else if(message.getID() == MessageID::SERIALIZE)
		{
			Serialization& serialization = *static_cast<Serialization*>(message.getParam());
			TARGET_SERIALIZER.serialize(serialization, _target);
		}
		else if(message.getID() == MessageID::DESERIALIZE)
		{
			const Serialization& serialization = *static_cast<const Serialization*>(message.getParam());
			TARGET_SERIALIZER.deserialize(serialization, _target);
		}
	}
	
	void LineRenderer::draw() const
	{
		const Point& position = *static_cast<Point*>(sendMessageToOwner(Message(MessageID::GET_POSITION)));
		Graphics::get().draw(Segment(position, _target), _color);
	}

	Component* LineRenderer::clone() const
	{
		return new LineRenderer(_layer, _color);
	}
}