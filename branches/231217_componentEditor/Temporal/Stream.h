#pragma once
#include <istream>

namespace Temporal
{
	class Stream
	{
	public:
		Stream(std::iostream* buffer) : _buffer(buffer) {};
		virtual ~Stream();

		virtual void close() {};
		void write(int value);
		void write(unsigned int value);
		void write(float value);
		void write(bool value);
		void write(const char* value);
		int readInt() const;
		unsigned int readUInt() const;
		float readFloat() const;
		bool readBool() const;
		const char* readString() const;
		std::string str() const;
		void copy(const Stream&);

	protected:
		std::iostream* _buffer;

	private:
		Stream(const Stream&);
		Stream& operator=(const Stream&);
	};
}

