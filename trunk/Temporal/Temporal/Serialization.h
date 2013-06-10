#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "tinyxml2.h"
#include <vector>
#include <unordered_map>
#include <sstream>

namespace Temporal
{
	/**********************************************************************************************
	 * Serialization direction
	 *********************************************************************************************/
	namespace SerializationDirection
	{
		enum Enum
		{
			SERIALIZATION,
			DESERIALIZATION
		};
	}

	/**********************************************************************************************
	 * Memory serializer
	 *********************************************************************************************/

	// Memory stream
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
	
	class SerializationAccess;
	class Hash;
	class Timer;

	// Memory base serializer
	class MemoryBaseSerializer
	{
	public:
		virtual ~MemoryBaseSerializer() {}
		virtual void serialize(const char* key, int& value) = 0;
		virtual void serialize(const char* key, unsigned int& value) = 0;
		virtual void serialize(const char* key, float& value) = 0;
		virtual void serialize(const char* key, bool& value) = 0;
		virtual void serialize(const char* key, Hash& value) = 0;
		virtual void serialize(const char* key, Timer& value) = 0;
		virtual void serializeRadians(const char* key, float& value) = 0;
		virtual SerializationDirection::Enum type() = 0;

		template<class T>
		void serialize(const char* key, T*& value)
		{
			SerializationAccess::serialize(key, *value, *this);
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
			for(TIterator i = value.begin(); i != value.end(); ++i)
				SerializationAccess::serialize(key, *i, *this);
		}
	protected:
		MemoryStream* _buffer;

		MemoryBaseSerializer(MemoryStream* buffer) : _buffer(buffer) {}

	private:
		MemoryBaseSerializer(const MemoryBaseSerializer&);
		MemoryBaseSerializer& operator=(const MemoryBaseSerializer&);
	};

	// Memory serializer;
	class MemorySerializer : public MemoryBaseSerializer
	{
	public:
		MemorySerializer(MemoryStream* buffer) : MemoryBaseSerializer(buffer) {};

		void serialize(const char* key, int& value) { _buffer->write(value); }
		void serialize(const char* key, unsigned int& value) { _buffer->write(value); }
		void serialize(const char* key, float& value) { _buffer->write(value); }
		void serialize(const char* key, bool& value) { _buffer->write(value); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { _buffer->write(value); };
		SerializationDirection::Enum type() { return SerializationDirection::SERIALIZATION; };
		
		template<class T>
		void serialize(const char* key, T*& value)
		{
			MemoryBaseSerializer::serialize(key,*value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			MemoryBaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			MemoryBaseSerializer::serialize(key, value);
		}
	};

	// Memory deserializer
	class MemoryDeserializer : public MemoryBaseSerializer
	{
	public:
		MemoryDeserializer(MemoryStream* buffer) : MemoryBaseSerializer(buffer) {};

		void serialize(const char* key, int& value) { value = _buffer->readInt(); }
		void serialize(const char* key, unsigned int& value) { value = _buffer->readUInt(); }
		void serialize(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, bool& value) { value = _buffer->readBool(); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { value = _buffer->readFloat(); }
		SerializationDirection::Enum type() { return SerializationDirection::DESERIALIZATION; };

		template<class T>
		void serialize(const char* key, T*& value)
		{
			MemoryBaseSerializer::serialize(key,*value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			MemoryBaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			MemoryBaseSerializer::serialize(key, value);
		}
	};

	/**********************************************************************************************
	 * Xml serializer
	 *********************************************************************************************/
	class XmlDeserializer
	{
	public:
		XmlDeserializer(const char * path);

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, char*& value);
		SerializationDirection::Enum type() { return SerializationDirection::DESERIALIZATION; };
		
		template<class T>
		void serialize(const char* key, T*& value)
		{
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if(current)
			{
				_current = current;
				SerializationAccess::serialize(_current->ToElement()->Attribute("type"), value, *this);
				_current = _current->Parent();
			}
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if(current)
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
			for(_current = _current->FirstChildElement(); _current; _current = _current->NextSiblingElement())
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
			for(_current = _current->FirstChildElement(); _current; _current = _current->NextSiblingElement())
			{
				T* object = 0;
				SerializationAccess::serialize(_current->Value(), object, *this);
				value[object->getId()] = object;
			}
			_current = parent;
		}

		
	private:
		tinyxml2::XMLDocument _doc; 
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
#endif