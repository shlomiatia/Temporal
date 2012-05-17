#ifndef BASEUTILS_H
#define BASEUTILS_H

#include "Hash.h"
#include "NumericPair.h"

namespace Temporal
{
	class Serialization;

	Hash createKey(const char* prefix, const char* name);

	class NumericPairSerializer
	{
	public:
		explicit NumericPairSerializer(const char* prefix) : _n1Key(createKey(prefix, "_N1")), _n2Key(createKey(prefix, "_N2")) {}

		void serialize(Serialization& serialization, const NumericPair& pair) const;
		void deserialize(const Serialization& deserialization, NumericPair& pair) const;
	private:
		const Hash _n1Key;
		const Hash _n2Key;

		NumericPairSerializer(const NumericPairSerializer&);
		NumericPairSerializer& operator=(const NumericPairSerializer&);
	};
}

#endif