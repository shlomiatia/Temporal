#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "Hash.h"
#include "tinyxml2.h"
#include <unordered_map>

namespace Temporal
{
	class Variant
	{
	public:
		Variant() : Int(0) {}
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
		Serialization() {};

		void serialize(const Hash& key, int value) { _serialization[key] = value; }
		void serialize(const Hash& key, unsigned int value) { _serialization[key] = value; }
		void serialize(const Hash& key, float value) { _serialization[key] = value; }
		void serialize(const Hash& key, bool value) { _serialization[key] = value; }

		int deserializeInt(const Hash& key) const { return _serialization.at(key).Int; }
		unsigned int deserializeUInt(const Hash& key) const { return _serialization.at(key).UInt; }
		float deserializeFloat(const Hash& key) const { return _serialization.at(key).Float; }
		bool deserializeBool(const Hash& key) const { return _serialization.at(key).Bool; }
	private:
		SerializationCollection _serialization;

		Serialization(const Serialization&);
		Serialization& operator=(const Serialization&);
	};

	class XmlDeserializer
	{
	public:
		XmlDeserializer(tinyxml2::XMLNode* root) : _current(root) {};

		template<class T>
		void serialize(const char* key, T& value)
		{
			_current = _current->FirstChildElement(key);
			value.serialize(*this);
			_current = _current->Parent();
		}

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
	private:
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
#endif