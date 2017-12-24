#include "Stream.h"
#include <sstream>

namespace Temporal
{
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
		_buffer->write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	}
	void Stream::write(float value)
	{
		_buffer->write(reinterpret_cast<const char*>(&value), sizeof(float));
	}
	void Stream::write(bool value)
	{
		_buffer->write(reinterpret_cast<const char*>(&value), sizeof(bool));
	}
	void Stream::write(const char* value)
	{
		int length = strlen(value) + 1;
		write(length);
		_buffer->write(value, sizeof(char)*length);
	}
	int Stream::readInt() const
	{
		int value;
		_buffer->read(reinterpret_cast<char*>(&value), sizeof(int));
		return value;
	}
	unsigned int Stream::readUInt() const
	{
		unsigned int value;
		_buffer->read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
		return value;
	}
	float Stream::readFloat() const
	{
		float value;
		_buffer->read(reinterpret_cast<char*>(&value), sizeof(float));
		return value;
	}
	bool Stream::readBool() const
	{
		bool value;
		_buffer->read(reinterpret_cast<char*>(&value), sizeof(bool));
		return value;
	}
	const char* Stream::readString() const
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

	std::string Stream::str() const
	{
		std::ostringstream buffer;
		std::copy(std::istreambuf_iterator<char>(*_buffer),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(buffer));
		return buffer.str();
	}
}
