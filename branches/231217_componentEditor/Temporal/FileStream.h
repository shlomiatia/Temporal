#pragma once
#include "Stream.h"

namespace Temporal
{
	class FileStream : public Stream
	{
	public:
		FileStream(const char* file, bool write, bool binary);
		void close();
	};
}
