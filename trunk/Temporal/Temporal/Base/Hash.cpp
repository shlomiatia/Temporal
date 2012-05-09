#include "Hash.h"

namespace Temporal
{
	const Hash Hash::INVALID = Hash("");

	Hash::Hash(const char* string)
	{
		_value = 5381;

		while(*string)
		{
			_value = ((_value << 5) + _value) + *string;
			++string;
		}
	}
}