#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "SerializationAccess.h"

namespace Temporal
{
	class ComponentEditor;
	class Hash;
	class Timer;
	
	class ComponentEditorSerializer
	{
	public:
		ComponentEditorSerializer(ComponentEditor& componentEditor) : _componentEditor(componentEditor) {}

		void serialize(const char* key, int& value) {};
		void serialize(const char* key, unsigned int& value) {};
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value) {};
		void serializeRadians(const char* key, float& value) {};
		void serialize(const char* key, std::string& value) {};

		template<class T>
		void serialize(const char* key, T*& value)
		{
			if (!SerializationAccess::shouldSerialize(value))
				return;

			SerializationAccess::modifyKey(key, value);

			if (strcmp(key, "sensor") == 0 || strcmp(key, "animator") == 0 || strcmp(key, "renderer") == 0)
				return;

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
		void preSerialize(const char* key);
		void postSerialize(const char* key);

		ComponentEditor& _componentEditor;
		std::vector<std::string> _prefixes;

		void hashChanged(const char* controlValue, Hash& componentValue);
		void boolChanged(bool controlValue, bool& componentValue);
		void floatChanged(const char* controlValue, float& componentValue);

		ComponentEditorSerializer(const ComponentEditorSerializer&);
		ComponentEditorSerializer& operator=(const ComponentEditorSerializer&);
	};
}
