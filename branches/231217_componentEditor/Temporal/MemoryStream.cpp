#include "MemoryStream.h"
#include <sstream>

namespace Temporal
{
	MemoryStream::MemoryStream() : Stream(new std::stringstream()) {}
	MemoryStream::MemoryStream(const char* data) : Stream(new std::stringstream(data)) {};
}
