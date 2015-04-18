#include "Serialization.h"
#include "Math.h"
#include "Hash.h"
#include "Timer.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iterator>

namespace Temporal
{
	/**********************************************************************************************
	 * Memory serializer
	 *********************************************************************************************/
	Stream::~Stream() 
	{
		delete _buffer; 
	}

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
	void Stream::write(const char* value)
	{
		int length = strlen(value) + 1;
		write(length);
		_buffer->write(value, sizeof(char)*length);
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
	const char* Stream::readString()
	{
		int length = readInt();
		char* value = new char[length];
		_buffer->read(value, length);
		return value;
	}
	void Stream::copy(const Stream& other)
	{
		std::copy(std::istreambuf_iterator<char>(*other._buffer),
				  std::istreambuf_iterator<char>(),
			      std::ostreambuf_iterator<char>(*_buffer));
	}

	std::string Stream::str()
	{
		std::ostringstream buffer;
		std::copy(std::istreambuf_iterator<char>(*_buffer),
				  std::istreambuf_iterator<char>(),
			      std::ostreambuf_iterator<char>(buffer));
		return buffer.str();
	}

	MemoryStream::MemoryStream() : Stream(new std::stringstream()) {}
	MemoryStream::MemoryStream(const char* data) : Stream(new std::stringstream(data)) {};

	FileStream::FileStream(const char* file, bool write, bool binary) : Stream(0)
	{
		std::fstream* buffer = new std::fstream();
		int mode = 0;
		if(binary)
			mode |= std::ios::binary;
		if(write)
			mode |= std::ios::out | std::ios::trunc;
		else
			mode |= std::ios::in;
		buffer->open(file, mode);
		_buffer = buffer;
	}
	void FileStream::close()
	{
		std::fstream* f = static_cast<std::fstream*>(_buffer);
		f->close();
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

	// Xml serializer
	XmlSerializer::XmlSerializer(Stream* stream) : BaseSerializer(stream), _current(&_doc) 
	{
	}

	XmlSerializer::~XmlSerializer()
	{
		_buffer->close();
		delete _buffer;
	}

	void XmlSerializer::serialize(const char* key, int& value)
	{
		_current->ToElement()->SetAttribute(key, value);
	}

	void XmlSerializer::serialize(const char* key, unsigned int& value)
	{
		_current->ToElement()->SetAttribute(key, value);
	}

	void XmlSerializer::serialize(const char* key, float& value)
	{
		_current->ToElement()->SetAttribute(key, value);
	}

	void XmlSerializer::serialize(const char* key, bool& value)
	{
		_current->ToElement()->SetAttribute(key, value);
	}

	void XmlSerializer::serialize(const char* key, Hash& value)
	{
		const char* str = value.getString();
		if(str)
			_current->ToElement()->SetAttribute(key, str);
	}

	void XmlSerializer::serialize(const char* key, std::string& value)
	{
		_current->ToElement()->SetAttribute(key, value.c_str());
	}

	void XmlSerializer::serialize(const char* key, Timer& value)
	{
		_current->ToElement()->SetAttribute(key, value.getElapsedTime());
	}

	void XmlSerializer::serializeRadians(const char* key, float& value)
	{
		_current->ToElement()->SetAttribute(key, AngleUtils::radiansToDegrees(value));
	}

	void XmlSerializer::save()
	{
		tinyxml2::XMLPrinter printer;
		_doc.Print(&printer);
		_buffer->copy(MemoryStream(printer.CStr()));
	}

	void XmlSerializer::preSerialize(const char* key)
	{
		tinyxml2::XMLElement* current = _doc.NewElement(key);
		_current->LinkEndChild(current);
		_current = current;
	}

	void XmlSerializer::postSerialize(const char* key)
	{
		_current = _current->Parent();
	}
		
	// Xml deserializer
	XmlDeserializer::XmlDeserializer(Stream* stream) : _current(0)
	{
		int result = _doc.Parse(stream->str().c_str());
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

	void XmlDeserializer::serialize(const char* key, std::string& value)
	{
		const char* valueP = _current->ToElement()->Attribute(key);
		if(valueP)
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