#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "tinyxml2.h"
#include <vector>
#include <unordered_map>
#include <map>

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
	class Stream
	{
	public:
		Stream(std::iostream* buffer) : _buffer(buffer) {};
		virtual ~Stream();

		virtual void close() {};
		void write(int value);
		void write(unsigned int value);
		void write(float value);
		void write(bool value);
		void write(const char* value);
		int readInt();
		unsigned int readUInt();
		float readFloat();
		bool readBool();
		const char* readString();
		std::string str();
		void copy(const Stream&);

	protected:
		std::iostream* _buffer;

	private:
		Stream(const Stream&);
		Stream& operator=(const Stream&);
	};

	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(const char* data);
	};

	class FileStream : public Stream
	{
	public:
		FileStream(const char* file, bool write, bool binary);
		void close();
	};
	
	class SerializationAccess;
	class Hash;
	class Timer;

	// Memory base serializer

	class BaseSerializer
	{
	public:
		virtual ~BaseSerializer() {}
		virtual void serialize(const char* key, int& value) = 0;
		virtual void serialize(const char* key, unsigned int& value) = 0;
		virtual void serialize(const char* key, float& value) = 0;
		virtual void serialize(const char* key, bool& value) = 0;
		virtual void serialize(const char* key, Hash& value) = 0;
		virtual void serialize(const char* key, Timer& value) = 0;
		virtual void serialize(const char* key, std::string& value) = 0;
		virtual void serializeRadians(const char* key, float& value) = 0;
		virtual SerializationDirection::Enum type() = 0;
		virtual void preSerialize(const char* key) {};
		virtual void postSerialize(const char* key) {};

		template<class T>
		void serialize(const char* key, T*& value)
		{
			bool shouldSerialize = true;
			SerializationAccess::getConfig(key, value, *this, shouldSerialize);
			if(!shouldSerialize)
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
			for(TIterator i = value.begin(); i != value.end(); ++i)
				serialize(key, *i);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for(TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
				serialize(key, i->second);
		}
	protected:
		Stream* _buffer;

		BaseSerializer(Stream* buffer) : _buffer(buffer) {}
		BaseSerializer(const Stream* buffer) : _buffer(const_cast<Stream*>(buffer)) {}

	private:
		BaseSerializer(const BaseSerializer&);
		BaseSerializer& operator=(const BaseSerializer&);
	};

	// Memory serializer;
	class BinarySerializer : public BaseSerializer
	{
	public:
		BinarySerializer(Stream* buffer) : BaseSerializer(buffer) {};

		void serialize(const char* key, int& value) { _buffer->write(value); }
		void serialize(const char* key, unsigned int& value) { _buffer->write(value); }
		void serialize(const char* key, float& value) { _buffer->write(value); }
		void serialize(const char* key, bool& value) { _buffer->write(value); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { _buffer->write(value); };
		void serialize(const char* key, std::string& value) { _buffer->write(value.c_str()); }
		SerializationDirection::Enum type() { return SerializationDirection::SERIALIZATION; };
		
		template<class T>
		void serialize(const char* key, T*& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			BaseSerializer::serialize(key, value);
		}
	};

	// Memory deserializer
	class BinaryDeserializer : public BaseSerializer
	{
	public:
		BinaryDeserializer(const Stream* buffer) : BaseSerializer(buffer) {};

		void serialize(const char* key, int& value) { value = _buffer->readInt(); }
		void serialize(const char* key, unsigned int& value) { value = _buffer->readUInt(); }
		void serialize(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, bool& value) { value = _buffer->readBool(); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { value = _buffer->readFloat(); }
		void serialize(const char* key, std::string& value) { value = _buffer->readString(); }
		SerializationDirection::Enum type() { return SerializationDirection::DESERIALIZATION; };

		template<class T>
		void serialize(const char* key, T*& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			BaseSerializer::serialize(key, value);
		}
	};

	/**********************************************************************************************
	 * Xml serializer
	 *********************************************************************************************/

	// XML serializer
	class XmlSerializer : public BaseSerializer
	{
	public:
		XmlSerializer(Stream* stream);
		~XmlSerializer();

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, std::string& value);
		SerializationDirection::Enum type() { return SerializationDirection::SERIALIZATION; };

		void preSerialize(const char* key);
		void postSerialize(const char* key);
		void save();

		template<class T>
		void serialize(const char* key, T*& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			BaseSerializer::serialize(key, value);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			BaseSerializer::serialize(key, value);
		}

	private:
		tinyxml2::XMLDocument _doc; 
		tinyxml2::XMLNode* _current;

		XmlSerializer(const XmlSerializer&);
		XmlSerializer& operator=(const XmlSerializer&);
	};

	// XML deserializer
	class XmlDeserializer
	{
	public:
		XmlDeserializer(Stream* stream);

		void serialize(const char* key, int& value);
		void serialize(const char* key, unsigned int& value);
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value);
		void serialize(const char* key, std::string& value);
		SerializationDirection::Enum type() { return SerializationDirection::DESERIALIZATION; };
		
		template<class T>
		void serialize(const char* key, T*& value)
		{
			tinyxml2::XMLNode* current = _current->FirstChildElement(key);
			if(current)
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
			for(_current = _current->FirstChildElement(key); _current; _current = _current->NextSiblingElement(key))
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