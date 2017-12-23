#pragma once
#include "Stream.h"

namespace Temporal
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(const char* data);
	};
}
