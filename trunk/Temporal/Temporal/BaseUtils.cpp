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

	void VectorSerializer::serialize(Serialization& serialization, const Vector& pair) const
	{
		serialization.serialize(_n1Key, pair.getX());
		serialization.serialize(_n2Key, pair.getY());
	}
	void VectorSerializer::deserialize(const Serialization& deserialization, Vector& pair) const
	{
		pair.setX(deserialization.deserializeFloat(_n1Key));
		pair.setY(deserialization.deserializeFloat(_n2Key));
	}
}