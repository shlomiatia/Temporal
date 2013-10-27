#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Temporal
{
	class Utils
	{
	public:
		static std::string toString(int i);
		static std::string toString(float f);
		static std::string format(const char* s, ...);
		static std::vector<std::string> split(const char* s, char d);
	};
}

#endif