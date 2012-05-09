#ifndef HASH_H
#define HASH_H

namespace Temporal
{
	class Hash
	{
	public:
		static const Hash INVALID;

		explicit Hash(const char* string);

		operator unsigned int() const { return _value; }
		bool operator==(const Hash& other) const { return _value == other._value; }
	private:
		unsigned int _value;
	};
}

#endif