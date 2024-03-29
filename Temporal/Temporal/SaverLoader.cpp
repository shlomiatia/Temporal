#include "SaverLoader.h"
#include "SerializationAccess.h"
#include "InputEnums.h"
#include "BinaryDeserializer.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "BinarySerializer.h"

namespace Temporal
{
	void EntitySaverLoader::handleMessage(Message& message)
	{
		if(message.getID() == MessageID::LOAD)
		{
			getEntity().handleMessage(Message(MessageID::PRE_LOAD));
			BinaryDeserializer* deserializer = static_cast<BinaryDeserializer*>(message.getParam());
			deserializer->serialize("entity", getEntity());
			getEntity().handleMessage(Message(MessageID::POST_LOAD));
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
		if (message.getID() == MessageID::ENTITY_INIT)
		{
			//getEntity().setBypassSave(true);
			getEntity().getManager().addInputComponent(this);
		}
		else if (message.getID() == MessageID::ENTITY_DISPOSED)
		{
			getEntity().getManager().removeInputComponent(this);
		}
		else if (message.getID() == MessageID::GAME_STATE_READY)
		{
			HashEntityMap& entities = getEntity().getManager().getEntities();
			Hash saverLoaderFilter = Hash("dynamic-body");
			for (HashEntityIterator i = entities.begin(); i != entities.end(); ++i)
			{
				Entity& entity = *i->second;
				if (entity.get(saverLoaderFilter))
					entity.add(new EntitySaverLoader());
			}
		}
		else if(message.getID() == MessageID::KEY_DOWN)
		{
			Key::Enum key = *static_cast<Key::Enum*>(message.getParam());
			if(!_loader.isStarted() && !_saver.isStarted())
			{
				if(key == Key::F5)
				{
					Stream* stream = new MemoryStream();
					BinarySerializer serializer(stream);
					getEntity().getManager().sendMessageToAllEntities(Message(MessageID::SAVE, &serializer));
					_saver.setStream(stream);
					IOThread::get().setJob(&_saver);
				}
				else if(key == Key::F9)
				{
					IOThread::get().setJob(&_loader);
				}
			}
		}
		else if(message.getID() == MessageID::UPDATE)
		{
			if(_saver.isFinished())
			{
				_saver.reset();
			}
			else if(_loader.isFinished())
			{
				_loader.reset();
				Stream* stream = _loader.getResult();
				BinaryDeserializer deserializer(stream);
				getEntity().getManager().sendMessageToAllEntities(Message(MessageID::LOAD, &deserializer));
				delete stream;
			}
		}
	}
}
