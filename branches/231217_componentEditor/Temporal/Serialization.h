#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include "tinyxml2.h"
#include <vector>
#include <unordered_map>
#include <map>

namespace Temporal
{
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
		int readInt() const;
		unsigned int readUInt() const;
		float readFloat() const;
		bool readBool() const;
		const char* readString() const;
		std::string str() const;
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

	// Memory serializer;
	class BinarySerializer
	{
	public:
		BinarySerializer(Stream* buffer) : _buffer(buffer) {}

		void serialize(const char* key, int& value) { _buffer->write(value); }
		void serialize(const char* key, unsigned int& value) { _buffer->write(value); }
		void serialize(const char* key, float& value) { _buffer->write(value); }
		void serialize(const char* key, bool& value) { _buffer->write(value); }
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value);
		void serializeRadians(const char* key, float& value) { _buffer->write(value); };
		void serialize(const char* key, std::string& value) { _buffer->write(value.c_str()); }

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
				serialize(key, *i);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for (TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
				serialize(key, i->second);
		}

	private:
		Stream* _buffer;

		BinarySerializer(const BinarySerializer&);
		BinarySerializer& operator=(const BinarySerializer&);
	};

	// Memory deserializer
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
		void serialize(const char* key, std::string& value) { value = _buffer->readString(); }

		virtual void preSerialize(const char* key) {};
		virtual void postSerialize(const char* key) {};

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
				serialize(key, *i);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for (TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
				serialize(key, i->second);
		}

	private:
		const Stream* _buffer;

		BinaryDeserializer(const BinarySerializer&);
		BinaryDeserializer& operator=(const BinarySerializer&);
	};

	/**********************************************************************************************
	 * Xml serializer
	 *********************************************************************************************/

	// XML serializer
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
				serialize(key, *i);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for (TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
				serialize(key, i->second);
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

	// XML deserializer
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
		Stream* _buffer;

		tinyxml2::XMLDocument _doc; 
		tinyxml2::XMLNode* _current;

		XmlDeserializer(const XmlDeserializer&);
		XmlDeserializer& operator=(const XmlDeserializer&);
	};
}
#endif