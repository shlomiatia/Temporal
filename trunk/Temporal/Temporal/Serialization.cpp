#include "Serialization.h"
#include "Math.h"
#include "Hash.h"
#include "Timer.h"
#include <cstring>

namespace Temporal
{
	/**********************************************************************************************
	 * Memory serializer
	 *********************************************************************************************/
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

	void MemorySerializer::serialize(const char* key, Hash& value)
	{
		_buffer->write(value); 
	}

	void MemorySerializer::serialize(const char* key, Timer& value)
	{
		_buffer->write(value.getElapsedTime());
	}

	void MemoryDeserializer::serialize(const char* key, Hash& value)
	{
		value = Hash(_buffer->readUInt()); 
	}

	void MemoryDeserializer::serialize(const char* key, Timer& value)
	{
		value.reset(_buffer->readFloat()); 
	}

	/**********************************************************************************************
	 * Xml serializer
	 *********************************************************************************************/
	XmlDeserializer::XmlDeserializer(const char * path) : _current(0) 
	{
		_doc.LoadFile(path);
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
		if(str)
			value = Hash(str);
	}

	void XmlDeserializer::serialize(const char* key, char*& value)
	{
		const char* str = _current->ToElement()->Attribute(key);
		value = new char[strlen(str)+1];
		strcpy(value, str);
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
		value = toRadians(degrees);
	}
}