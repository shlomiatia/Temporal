#pragma once
#include "Stream.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class Timer;
	class Hash;

	class BinaryDeserializer
	{
	public:
		BinaryDeserializer(const Stream* buffer) : _buffer(buffer) {}

		void serialize(const char* key, int& value) { value = _buffer->readInt(); }
		void serialize(const char* key, unsigned int& value) { value = _buffer->readUInt(); }
		void serialize(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, bool& value) { value = _buffer->readBool(); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, std::string& value) { }

		template<class T>
		void serialize(const char* key, T*& value)
		{
			if (!SerializationAccess::shouldSerialize(value))
				return;

			SerializationAccess::serialize(key, value, *this);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			SerializationAccess::serialize(key, value, *this);
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
		const Stream* _buffer;

		BinaryDeserializer(const BinaryDeserializer&);
		BinaryDeserializer& operator=(const BinaryDeserializer&);
	};
}
