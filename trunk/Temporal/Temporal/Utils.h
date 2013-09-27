#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Temporal
{
	class Utils
	{
	public:
		static std::string toString(int i);
		static std::string toString(float f);
		static std::string format(const char* s, ...);
	};
}

#endif