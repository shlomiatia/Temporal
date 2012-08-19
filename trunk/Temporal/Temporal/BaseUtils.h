#ifndef BASEUTILS_H
#define BASEUTILS_H

#include "Hash.h"
#include "Vector.h"

namespace Temporal
{
	class Serialization;

	Hash createKey(const char* prefix, const char* name);

	class VectorSerializer
	{
	public:
		explicit VectorSerializer(const char* prefix) : _n1Key(createKey(prefix, "_N1")), _n2Key(createKey(prefix, "_N2")) {}

		void serialize(Serialization& serialization, const Vector& pair) const;
		void deserialize(const Serialization& deserialization, Vector& pair) const;
	private:
		const Hash _n1Key;
		const Hash _n2Key;

		VectorSerializer(const VectorSerializer&);
		VectorSerializer& operator=(const VectorSerializer&);
	};
}

#endif