#include "EdgeDetector.h"

namespace Temporal
{
	void EdgeDetector::start()
	{
		_isFound = true;
	}

	void EdgeDetector::handle(const Contact& contact)
	{
		_isFound = false;
	}

	void EdgeDetector::end()
	{
		if (_isFound)
		{
			getOwner().raiseMessage(Message(MessageID::SENSOR_FRONG_EDGE));
		}
	}
}