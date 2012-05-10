#ifndef BASEUTILS_H
#define BASEUTILS_H

#include "Hash.h"
#include "NumericPair.h"

namespace Temporal
{
	class Serialization;

	class NumericPairSerializer
	{
	public:
		explicit NumericPairSerializer(const char* prefix) : _n1Key(getKey(prefix, "_N1")), _n2Key(getKey(prefix, "_N2")) {}

		void serialize(Serialization& serialization, const NumericPair& pair) const;
		void deserialize(const Serialization& deserialization, NumericPair& pair) const;
	private:
		const Hash _n1Key;
		const Hash _n2Key;

		Hash getKey(const char* prefix, const char* name);

		NumericPairSerializer(const NumericPairSerializer&);
		NumericPairSerializer& operator=(const NumericPairSerializer&);
	};
}

#endif