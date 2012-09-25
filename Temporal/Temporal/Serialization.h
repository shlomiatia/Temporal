#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "Hash.h"
#include "Timer.h"
#include "tinyxml2.h"
#include <vector>
#include <unordered_map>
#include <sstream>

namespace Temporal
{
	namespace SerializationType
	{
		enum Enum
		{
			SERIALIZATION,
			DESERIALIZATION
		};
	}

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

	class MemorySerialization
	{
	public:
		virtual ~MemorySerialization() {}
		virtual void serialize(const char* key, int& value) = 0;
		virtual void serialize(const char* key, unsigned int& value) = 0;
		virtual void serialize(const char* key, float& value) = 0;
		virtual void serialize(const char* key, bool& value) = 0;
		virtual void serialize(const char* key, Hash& value) = 0;
		virtual void serialize(const char* key, Timer& value) = 0;
		virtual void serializeRadians(const char* key, float& value) = 0;
		virtual SerializationType::Enum type() = 0;

		template<class T>
		void serialize(const char* key, T*& value)
		{
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
			for(TIterator i = value.begin(); i != value.end(); ++i)
				SerializationAccess::serialize(key, *i, *this);
		}
	protected:
		MemorySerialization(MemoryStream* buffer) : _buffer(buffer) {}

		MemoryStream* _buffer;		
	private:
		MemorySerialization(const MemorySerialization&);
		MemorySerialization& operator=(const MemorySerialization&);
	};

	class MemorySerializer : public MemorySerialization
	{
	public:
		MemorySerializer(MemoryStream* buffer) : MemorySerialization(buffer) {};

		void serialize(const char* key, int& value) { _buffer->write(value); }
		void serialize(const char* key, unsigned int& value) { _buffer->write(value); }
		void serialize(const char* key, float& value) { _buffer->write(value); }
		void serialize(const char* key, bool& value) { _buffer->write(value); }
		void serialize(const char* key, Hash& value) { _buffer->write(value); }
		void serialize(const char* key, Timer& value) { _buffer->write(value.getElapsedTime()); }
		void serializeRadians(const char* key, float& value) { _buffer->write(value); };
		SerializationType::Enum type() { return SerializationType::SERIALIZATION; };
		
		template<class T>
		void serialize(const char* key, T*& value)
		{
			MemorySerialization::serialize(key,*value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			MemorySerialization::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			MemorySerialization::serialize(key, value);
		}
	};

	class MemoryDeserializer : public MemorySerialization
	{
	public:
		MemoryDeserializer(MemoryStream* buffer) : MemorySerialization(buffer) {};

		void serialize(const char* key, int& value) { value = _buffer->readInt(); }
		void serialize(const char* key, unsigned int& value) { value = _buffer->readUInt(); }
		void serialize(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, bool& value) { value = _buffer->readBool(); }
		void serialize(const char* key, Hash& value)  { value = Hash(_buffer->readUInt()); }
		void serialize(const char* key, Timer& value) { value.reset(_buffer->readFloat()); }
		void serializeRadians(const char* key, float& value) { value = _buffer->readFloat(); }
		SerializationType::Enum type() { return SerializationType::DESERIALIZATION; };

		template<class T>
		void serialize(const char* key, T*& value)
		{
			MemorySerialization::serialize(key,*value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			MemorySerialization::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			MemorySerialization::serialize(key, value);
		}
	};

	class XmlDeserializer
	{
	public:
		XmlDeserializer(const char * path) : _current(0) { _doc.LoadFile(path); _current = _doc.GetDocument(); };

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		SerializationType::Enum type() { return SerializationType::DESERIALIZATION; };

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