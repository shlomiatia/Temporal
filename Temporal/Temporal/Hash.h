#ifndef HASH_H
#define HASH_H

#include <unordered_map>

namespace Temporal
{
	class Hash
	{
	public:
		static const Hash INVALID;

		explicit Hash(const char* string);		
		explicit Hash(unsigned int value = 0) : _value(value) {}

		const char* getString() const;

		operator unsigned int() const { return _value; }
		bool operator==(Hash other) const { return _value == other._value; }
	private:
		unsigned int _value;
	};

	typedef std::unordered_map<Hash, const char*> HashCollection;
	typedef HashCollection::const_iterator HashIterator;

	class HashToString
	{
	public:
		static HashToString& get()
		{
			static HashToString instance;
			return instance;
		}

		// TODO:
		void dispose() const;

		void save(Hash key, const char* value);
		const char* getString(Hash hash) const;
	private:
		HashCollection _hashStrings;

		HashToString() {}
		HashToString(const HashToString&);
		HashToString& operator=(const HashToString&);
	};
}

#endif