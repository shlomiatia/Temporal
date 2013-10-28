#include "Utils.h"
#include <sstream>
#include <cstdarg>

namespace Temporal
{
	std::string Utils::toString(int i)
	{
		std::ostringstream s;
		s << i;
		return s.str();
	}

	std::string Utils::toString(float f)
	{
		std::ostringstream s;
		s << f;
		return s.str();
	}

	int Utils::parseInt(const char* s)
	{
		std::istringstream ss(s);
		int i = 0;
		ss >> i;
		return i;
	}

	float Utils::parseFloat(const char* s)
	{
		std::istringstream ss(s);
		float f = 0;
		ss >> f;
		return f;
	}

	std::string Utils::format(const char* s, ...)
	{
		const char* ts;
		int n;
		float f;
		bool flag = false;

		va_list arguments;
		va_start(arguments, s);
		std::ostringstream ss;
		int i = 0;
		char ch = s[i];
		while(ch)
		{
			if(!flag)
			{
				if(ch == '%')
					flag = true;
				else
					ss << ch;
			}
			else
			{
				switch(ch)
				{
					case 's': { ts = va_arg(arguments, const char*);  ss << ts; } break;
					case 'c': { n = va_arg(arguments, int);    ss << n;   } break;
					case 'd': { n = va_arg(arguments, int);    ss << n;    } break;
					case 'f': { f = va_arg(arguments, float); ss << f;   } break;
					case '%': { ss << '%'; } break;
					default:  { abort(); }
				}
				flag = false;
			}
			++i;
			ch = s[i];
		}

		va_end(arguments);
		return ss.str();
	}

	std::vector<std::string> Utils::split(const char* s, char d) 
	{
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> result;
		while (std::getline(ss, item, d)) {
			result.push_back(item);
		}
		return result;
	}
}