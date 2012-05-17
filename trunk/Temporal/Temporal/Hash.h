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
		explicit Hash(unsigned int value) : _value(value) {}

		operator unsigned int(void) const { return _value; }
		bool operator==(const Hash& other) const { return _value == other._value; }
	private:
		unsigned int _value;
	};

	typedef std::unordered_map<const Hash, const char*> HashCollection;
	typedef HashCollection::const_iterator HashIterator;

	class HashToString
	{
	public:
		static HashToString& get(void)
		{
			static HashToString instance;
			return instance;
		}

		void save(const Hash& key, const char* value);
		const char* getString(const Hash& hash) const;
	private:
		HashCollection _hashStrings;

		HashToString(void) {}
		HashToString(const HashToString&);
		HashToString& operator=(const HashToString&);
	};
}

#endif