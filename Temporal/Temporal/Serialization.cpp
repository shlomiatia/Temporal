#include "Serialization.h"
#include "Math.h"
#include "Hash.h"
#include "Timer.h"
#include <cstring>
#include <fstream>

namespace Temporal
{
	/**********************************************************************************************
	 * Memory serializer
	 *********************************************************************************************/
	void Stream::write(int value)
	{
		_buffer->write((const char*)&value, sizeof(int));
	}
	void Stream::write(unsigned int value)
	{
		_buffer->write((const char*)&value, sizeof(unsigned int));
	}
	void Stream::write(float value)
	{
		_buffer->write((const char*)&value, sizeof(float));
	}
	void Stream::write(bool value)
	{
		_buffer->write((const char*)&value, sizeof(bool));
	}
	int Stream::readInt()
	{
		int value;
		_buffer->read((char*)&value, sizeof(int));
		return value;
	}
	unsigned int Stream::readUInt()
	{
		unsigned int value;
		_buffer->read((char*)&value, sizeof(unsigned int));
		return value;
	}
	float Stream::readFloat()
	{
		float value;
		_buffer->read((char*)&value, sizeof(float));
		return value;
	}
	bool Stream::readBool()
	{
		bool value;
		_buffer->read((char*)&value, sizeof(bool));
		return value;
	}

	MemoryStream::MemoryStream() : Stream(new std::stringstream()) {}

	FileStream::FileStream(const char* file) : Stream(0)
	{
		std::fstream* buffer = new std::fstream();
		buffer->open(file);
		_buffer = buffer;
	}

	void BinarySerializer::serialize(const char* key, Hash& value)
	{
		_buffer->write(value); 
	}

	void BinarySerializer::serialize(const char* key, Timer& value)
	{
		_buffer->write(value.getElapsedTime());
	}

	void BinaryDeserializer::serialize(const char* key, Hash& value)
	{
		value = Hash(_buffer->readUInt()); 
	}

	void BinaryDeserializer::serialize(const char* key, Timer& value)
	{
		value.reset(_buffer->readFloat()); 
	}

	/**********************************************************************************************
	 * Xml serializer
	 *********************************************************************************************/
	XmlDeserializer::XmlDeserializer(const char * path) : _current(0) 
	{
		int result = _doc.LoadFile(path);
		if(result != 0)
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
		if(str)
			value = Hash(str);
	}

	void XmlDeserializer::serialize(const char* key, char*& value)
	{
		const char* str = _current->ToElement()->Attribute(key);
		if(str)
		{
			value = new char[strlen(str)+1];
			strcpy(value, str);
		}
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