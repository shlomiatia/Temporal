#include "BaseUtils.h"
#include "Serialization.h"
#include <sstream>

namespace Temporal
{
	Hash createKey(const char* prefix, const char* name)
	{
		std::ostringstream buffer;
		buffer << prefix << name;
		std::string str = buffer.str();
		const char* key = str.c_str();
		return Hash(key);
	}

	void NumericPairSerializer::serialize(Serialization& serialization, const NumericPair& pair) const
	{
		serialization.serialize(_n1Key, pair.getN1());
		serialization.serialize(_n2Key, pair.getN2());
	}
	void NumericPairSerializer::deserialize(const Serialization& deserialization, NumericPair& pair) const
	{
		pair.setN1(deserialization.deserializeFloat(_n1Key));
		pair.setN2(deserialization.deserializeFloat(_n2Key));
	}
}