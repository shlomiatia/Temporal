#include "XmlSerializer.h"
#include "Stream.h"
#include "Hash.h"
#include "Math.h"
#include "Timer.h"
#include "MemoryStream.h"

namespace Temporal
{
	XmlSerializer::XmlSerializer(Stream* buffer) : _buffer(buffer), _current(&_doc)
	{
	}

	XmlSerializer::~XmlSerializer()
	{
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
		if (str)
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
		_buffer->close();
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
}
