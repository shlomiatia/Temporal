#include "BinarySerializer.h"
#include "Timer.h"

namespace Temporal
{
	void BinarySerializer::serialize(const char* key, Hash& value)
	{
		_buffer->write(value);
	}

	void BinarySerializer::serialize(const char* key, Timer& value)
	{
		_buffer->write(value.getElapsedTime());
	}
}
