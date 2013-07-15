#include "SaverLoader.h"
#include "Serialization.h"
#include "SerializationAccess.h"
#include "Keyboard.h"
#include "Log.h"

namespace Temporal
{
	const Hash EntitySaverLoader::TYPE = Hash("saver-loader");
	const Hash GameSaverLoader::TYPE = Hash("game-saver-loader");

	void EntitySaverLoader::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::LOAD)
		{
			BinaryDeserializer* deserializer = static_cast<BinaryDeserializer*>(message.getParam());
			deserializer->serialize("entity", getEntity());
		}
		else if(message.getID() == MessageID::SAVE)
		{
			BinarySerializer* serializer = static_cast<BinarySerializer*>(message.getParam());
			serializer->serialize("entity", getEntity());
		}
	}

	void GameLoader::executeImpl()
	{
		FileStream fileStream(_path, false, true);
		_result = new MemoryStream();
		_result->copy(fileStream);
		fileStream.close();
	}

	void GameSaver::executeImpl()
	{
		FileStream fileStream(_path, true, true);
		fileStream.copy(*_stream);
		fileStream.close();
		delete _stream;
	}

	void GameSaverLoader::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::UPDATE)
		{
			if(!_loader.isStarted() && !_saver.isStarted())
			{
				if(Keyboard::get().getKey(Key::F5))
				{
					Stream* stream = new MemoryStream();
					BinarySerializer serializer(stream);
					getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SAVE, &serializer));
					//serializer.save();
					_saver.setStream(stream);
					IOThread::get().setJob(&_saver);
				}
				else if(Keyboard::get().getKey(Key::F9))
				{
					IOThread::get().setJob(&_loader);
				}
			}
			else if(_saver.isFinished())
			{
				_saver.reset();
			}
			else if(_loader.isFinished())
			{
				_loader.reset();
				Stream* stream = _loader.getResult();
				BinaryDeserializer deserializer(stream);
				//deserializer.crapMode = true;
				getEntity().getManager().sendMessageToAllEntities(Message(MessageID::LOAD, &deserializer));
				delete stream;
			}
		}
	}
}