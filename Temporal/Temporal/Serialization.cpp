#include "Serialization.h"

namespace Temporal
{
	void XmlDeserializer::serialize(const char* key, int& value)
	{
		value = _current->ToElement()->IntAttribute(key);
	}

	void XmlDeserializer::serialize(const char* key, unsigned int& value)
	{
		value = _current->ToElement()->UnsignedAttribute(key);
	}

	void XmlDeserializer::serialize(const char* key, float& value)
	{
		value = _current->ToElement()->FloatAttribute(key);
	}

	void XmlDeserializer::serialize(const char* key, bool& value)
	{
		value = _current->ToElement()->BoolAttribute(key);
	}

	void XmlDeserializer::serialize(const char* key, const char** value)
	{
		*value = _current->ToElement()->Attribute(key);
	}

	void XmlDeserializer::serialize(const char* key, Hash& value)
	{
		const char* str = _current->ToElement()->Attribute(key);
		if(str)
			value = Hash(str);
	}
}