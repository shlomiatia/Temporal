#include "Serialization.h"
#include "Math.h"

namespace Temporal
{
	void MemoryStream::write(int value)
	{
		_buffer.write((const char*)&value, sizeof(int));
	}
	void MemoryStream::write(unsigned int value)
	{
		_buffer.write((const char*)&value, sizeof(unsigned int));
	}
	void MemoryStream::write(float value)
	{
		_buffer.write((const char*)&value, sizeof(float));
	}
	void MemoryStream::write(bool value)
	{
		_buffer.write((const char*)&value, sizeof(bool));
	}
	int MemoryStream::readInt()
	{
		int value;
		_buffer.read((char*)&value, sizeof(int));
		return value;
	}
	unsigned int MemoryStream::readUInt()
	{
		unsigned int value;
		_buffer.read((char*)&value, sizeof(unsigned int));
		return value;
	}
	float MemoryStream::readFloat()
	{
		float value;
		_buffer.read((char*)&value, sizeof(float));
		return value;
	}
	bool MemoryStream::readBool()
	{
		bool value;
		_buffer.read((char*)&value, sizeof(bool));
		return value;
	}

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

	void XmlDeserializer::serializeRadians(const char* key, float& value)
	{
		float degrees = 0.0f;
		serialize(key, degrees);
		value = toRadians(degrees);
	}
}