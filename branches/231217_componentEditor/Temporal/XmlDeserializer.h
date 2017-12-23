#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "tinyxml2.h"

namespace Temporal
{
	class Timer;
	class Hash;
	class Stream;

	class XmlDeserializer
	{
	public:
		XmlDeserializer(Stream* buffer);

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, std::string& value);

		template<class T>
		void serialize(const char* key, T*& value)
		{
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if (current)
			{
				_current = current;
				SerializationAccess::serialize(_current->Value(), value, *this);
				_current = _current->Parent();
			}
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if (current)
			{
				_current = current;
				SerializationAccess::serialize(_current->Value(), value, *this);
				_current = _current->Parent();
			}
		}

		template<typename T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			tinyxml2::XMLNode* parent = _current;
			for (_current = _current->FirstChildElement(key); _current; _current = _current->NextSiblingElement(key))
			{
				T* object = 0;
				SerializationAccess::serialize(_current->Value(), object, *this);
				value.push_back(object);
			}
			_current = parent;
		}

		template<typename T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			tinyxml2::XMLNode* parent = _current;
			for (_current = _current->FirstChildElement(); _current; _current = _current->NextSiblingElement())
			{
				T* object = 0;
				SerializationAccess::serialize(_current->Value(), object, *this);
				value[object->getId()] = object;
			}
			_current = parent;
		}

	private:
		Stream* _buffer;

		tinyxml2::XMLDocument _doc;
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
