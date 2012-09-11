#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "Hash.h"
#include "tinyxml2.h"
#include <vector>
#include <unordered_map>
#include <sstream>

namespace Temporal
{
	class MemoryStream
	{
	public:
		MemoryStream() {};

		void write(int value);
		void write(unsigned int value);
		void write(float value);
		void write(bool value);
		int readInt();
		unsigned int readUInt();
		float readFloat();
		bool readBool();
	private:
		std::stringstream _buffer;

		MemoryStream(const MemoryStream&);
		MemoryStream& operator=(const MemoryStream&);
	};

	class Serializer;

	class Serializable
	{
		public:
			virtual void serialize(Serializer& serializer) = 0;
	};

	class Serializer
	{
	public:
		virtual void serialize(const char* key, int& value) = 0;
		virtual void serialize(const char* key, unsigned int& value) = 0;
		virtual void serialize(const char* key, float& value) = 0;
		virtual void serialize(const char* key, bool& value) = 0;
		virtual void serialize(const char* key, Hash& value) = 0;
		virtual void serializeRadians(const char* key, float& value) = 0;
		virtual void serialize(const char* key, const char** value) = 0;

		virtual void serialize(const char* key, Serializable& value) = 0;

	};

	class MemorySerializer
	{
	public:
		MemorySerializer(MemoryStream* buffer) : _buffer(buffer) {};

		void serialize(const char* key, int& value) { _buffer->write(value); }
		void serialize(const char* key, unsigned int& value) { _buffer->write(value); }
		void serialize(const char* key, float& value) { _buffer->write(value); }
		void serialize(const char* key, bool& value) { _buffer->write(value); }
		void serialize(const char* key, Hash& value) { _buffer->write(value); }
		void serializeRadians(const char* key, float& value) { _buffer->write(value); };

		template<class T>
		void serialize(const char* key, T& value)
		{
			value.serialize(*this);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			typedef std::vector<T*>::const_iterator TIterator;
			for(TIterator i = value.begin(); i != value.end(); ++i)
				(**i).serialize(*this);
		}
	private:
		MemoryStream* _buffer;

		MemorySerializer(const MemorySerializer&);
		MemorySerializer& operator=(const MemorySerializer&);
	};

	class MemoryDeserializer
	{
	public:
		MemoryDeserializer(MemoryStream* buffer) : _buffer(buffer) {};

		void serialize(const char* key, int& value) { value = _buffer->readInt(); }
		void serialize(const char* key, unsigned int& value) { value = _buffer->readUInt(); }
		void serialize(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, bool& value) { value = _buffer->readBool(); }
		void serialize(const char* key, Hash& value)  { value = Hash(_buffer->readUInt()); }
		void serializeRadians(const char* key, float& value) { value = _buffer->readFloat(); }

		template<class T>
		void serialize(const char* key, T& value)
		{
			value.serialize(*this);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			typedef std::vector<T*>::const_iterator TIterator;
			for(TIterator i = value.begin(); i != value.end(); ++i)
				(**i).serialize(*this);
		}
	private:
		MemoryStream* _buffer;

		MemoryDeserializer(const MemoryDeserializer&);
		MemoryDeserializer& operator=(const MemoryDeserializer&);
	};

	class XmlDeserializer
	{
	public:
		XmlDeserializer(tinyxml2::XMLNode* root) : _current(root) {};

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, const char** value);

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

		
	private:
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
#endif