#include "FileStream.h"
#include <fstream>

namespace Temporal
{
	FileStream::FileStream(const char* file, bool write, bool binary) : Stream(0)
	{
		std::fstream* buffer = new std::fstream();
		int mode = 0;

		if (binary)
		{
			mode |= std::ios::binary;
		}
			
		if (write)
		{
			mode |= std::ios::out | std::ios::trunc;
		}
		else
		{
			mode |= std::ios::in;
		}
			
		buffer->open(file, mode);
		_buffer = buffer;
	}

	void FileStream::close()
	{
		std::fstream* f = static_cast<std::fstream*>(_buffer);
		f->close();
	}
}
