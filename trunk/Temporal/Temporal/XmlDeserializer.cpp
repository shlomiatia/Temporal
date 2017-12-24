#include "XmlDeserializer.h"
#include "Math.h"
#include "Stream.h"
#include "Hash.h"
#include "Timer.h"

namespace Temporal
{
	XmlDeserializer::XmlDeserializer(Stream* buffer) : _buffer(buffer), _current(0)
	{
		int result = _doc.Parse(buffer->str().c_str());
		if (result != 0)
			abort();
		_current = _doc.GetDocument();
	};

	void XmlDeserializer::serialize(const char* key, int& value)
	{
		_current->ToElement()->QueryIntAttribute(key, &value);
	}

	void XmlDeserializer::serialize(const char* key, unsigned int& value)
	{
		_current->ToElement()->QueryUnsignedAttribute(key, &value);
	}

	void XmlDeserializer::serialize(const char* key, float& value)
	{
		_current->ToElement()->QueryFloatAttribute(key, &value);
	}

	void XmlDeserializer::serialize(const char* key, bool& value)
	{
		_current->ToElement()->QueryBoolAttribute(key, &value);
	}

	void XmlDeserializer::serialize(const char* key, Hash& value)
	{
		const char* str = _current->ToElement()->Attribute(key);
		if (str)
			value = Hash(str);
	}

	void XmlDeserializer::serialize(const char* key, std::string& value)
	{
		const char* valueP = _current->ToElement()->Attribute(key);
		if (valueP)
			value = valueP;
	}

	void XmlDeserializer::serialize(const char* key, Timer& value)
	{
		float time = 0.0f;
		serialize(key, time);
		value.reset(time);
	}

	void XmlDeserializer::serializeRadians(const char* key, float& value)
	{
		float degrees = 0.0f;
		serialize(key, degrees);
		value = AngleUtils::degreesToRadians(degrees);
	}
}
