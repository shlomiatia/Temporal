#ifndef HASH_H
#define HASH_H

namespace Temporal
{
	// TODO: Global string list
	class Hash
	{
	public:
		static const Hash INVALID;

		explicit Hash(const char* string);
		explicit Hash(unsigned int value) : _value(value) {}

		operator unsigned int() const { return _value; }
		bool operator==(const Hash& other) const { return _value == other._value; }
	private:
		unsigned int _value;
	};
}

#endif