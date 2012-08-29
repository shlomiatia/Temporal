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

		const char* getString() const;

		operator unsigned int() const { return _value; }
		bool operator==(const Hash& other) const { return _value == other._value; }

	private:
		unsigned int _value;
	};

	typedef std::unordered_map<const Hash, const char*> HashCollection;
	typedef HashCollection::const_iterator HashIterator;

	class HashToString
	{
	public:
		static HashToString& get()
		{
			static HashToString instance;
			return instance;
		}
		void dispose() const;

		void save(const Hash& key, const char* value);
		const char* getString(const Hash& hash) const;
	private:
		HashCollection _hashStrings;

		HashToString() {}
		HashToString(const HashToString&);
		HashToString& operator=(const HashToString&);
	};
}

#endif