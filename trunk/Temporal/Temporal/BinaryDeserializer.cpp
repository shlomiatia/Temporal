#include "BinaryDeserializer.h"
#include "Hash.h"
#include "Timer.h"

namespace Temporal
{
	void BinaryDeserializer::serialize(const char* key, Hash& value)
	{
		value = Hash(_buffer->readUInt());
	}

	void BinaryDeserializer::serialize(const char* key, Timer& value)
	{
		value.reset(_buffer->readFloat());
	}
}
