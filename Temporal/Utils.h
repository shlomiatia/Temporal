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
		static int parseInt(const char* s);
		static float parseFloat(const char* s);
		static std::string format(const char* s, ...);
		static std::vector<std::string> split(const char* s, char d);
		static std::string join(std::vector<std::string> strings, const char* d);
	};
}

#endif