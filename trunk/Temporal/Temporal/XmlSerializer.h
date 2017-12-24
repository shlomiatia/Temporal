#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include "tinyxml2.h"

namespace Temporal
{
	class Timer;
	class Hash;
	class Stream;

	class XmlSerializer
	{
	public:
		XmlSerializer(Stream* buffer);
		~XmlSerializer();

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, std::string& value);
		void save();

		template<class T>
		void serialize(const char* key, T*& value)
		{
			if (!SerializationAccess::shouldSerialize(value))
				return;

			SerializationAccess::modifyKey(key, value);
			preSerialize(key);
			SerializationAccess::serialize(key, value, *this);
			postSerialize(key);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			preSerialize(key);
			SerializationAccess::serialize(key, value, *this);
			postSerialize(key);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			typedef std::vector<T*>::iterator TIterator;
			for (TIterator i = value.begin(); i != value.end(); ++i)
			{
				serialize(key, *i);
			}
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for (TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
			{
				serialize(key, i->second);
			}
		}

	private:
		Stream* _buffer;

		tinyxml2::XMLDocument _doc;
		tinyxml2::XMLNode* _current;

		void preSerialize(const char* key);
		void postSerialize(const char* key);

		XmlSerializer(const XmlSerializer&);
		XmlSerializer& operator=(const XmlSerializer&);
	};
}
