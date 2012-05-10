#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "Hash.h"
#include <unordered_map>

namespace Temporal
{
	class Variant
	{
	public:
		Variant(void) : Int(0) {}
		Variant(int value) : Int(value) {}
		Variant(unsigned int value) : UInt(value) {}
		Variant(float value) : Float(value) {}
		Variant(bool value) : Bool(value) {}

		union
		{
			int Int;
			unsigned int UInt;
			float Float;
			bool Bool;
		};
	};

	typedef std::unordered_map<const Hash, Variant> SerializationCollection;

	class Serialization
	{
	public:
		void serialize(const Hash& key, int value) { _serialization[key] = value; }
		void serialize(const Hash& key, unsigned int value) { _serialization[key] = value; }
		void serialize(const Hash& key, float value) { _serialization[key] = value; }
		void serialize(const Hash& key, bool value) { _serialization[key] = value; }

		int deserializeInt(const Hash& key) const { return _serialization[key].Int; }
		unsigned int deserializeUInt(const Hash& key) const { return _serialization[key].UInt; }
		float deserializeFloat(const Hash& key) const { return _serialization[key].Float; }
		bool deserializeBool(const Hash& key) const { return _serialization[key].Bool; }
	private:
		mutable SerializationCollection _serialization;
	};
}
#endif