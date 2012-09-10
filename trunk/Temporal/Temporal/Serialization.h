#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "Hash.h"
#include "tinyxml2.h"
#include <unordered_map>
#include <vector>
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

	typedef std::unordered_map<Hash, Variant> SerializationCollection;

	class Serialization
	{
	public:
		Serialization() {};

		void serialize(Hash key, int value) { _serialization[key] = value; }
		void serialize(Hash key, unsigned int value) { _serialization[key] = value; }
		void serialize(Hash key, float value) { _serialization[key] = value; }
		void serialize(Hash key, bool value) { _serialization[key] = value; }

		int deserializeInt(Hash key) const { return _serialization.at(key).Int; }
		unsigned int deserializeUInt(Hash key) const { return _serialization.at(key).UInt; }
		float deserializeFloat(Hash key) const { return _serialization.at(key).Float; }
		bool deserializeBool(Hash key) const { return _serialization.at(key).Bool; }
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
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if(current != NULL)
			{
				_current = current;
				value.serialize(*this);
				_current = _current->Parent();
			}
		}

		template<typename T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			tinyxml2::XMLNode* parent = _current;
			for(_current = _current->FirstChildElement(key); _current != NULL; _current = _current->NextSiblingElement())
			{
				T* object = new T();
				object->serialize(*this);
				value.push_back(object);
			}
			_current = parent;
		}

		template<typename K, typename V>
		void serialize(const char* key, std::unordered_map<K, V*>& value)
		{
			tinyxml2::XMLNode* parent = _current;
			for(_current = _current->FirstChildElement(key); _current != NULL; _current = _current->NextSiblingElement())
			{
				V* object = new V();
				object->serialize(*this);
				value[object->getId()] = object;
			}
			_current = parent;
		}

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, const char** value);
		void serialize(const char* key, Hash& value);
	private:
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
#endif