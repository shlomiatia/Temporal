#ifndef SAVERLOADER_H
#define SAVERLOADER_H

#include "EntitySystem.h"
#include "ResourceManager.h"
#include "Ids.h"

namespace Temporal
{
	class EntitySaverLoader : public Component
	{
	public:
		EntitySaverLoader() : Component(true) {}
		void handleMessage(Message& message);
		Hash getType() const { return ComponentsIds::ENTITY_SAVER_LOADER; }
		Component* clone() const { return new EntitySaverLoader(); }
		
	private:
	};

	class GameLoader : public IOJob
	{
	public:
		GameLoader(const char* path) : _path(path) {}
		void executeImpl();

		Stream* getResult() { return _result; }

	private:
		const char* _path;
		Stream* _result;
	};

	class GameSaver : public IOJob
	{
	public:
		GameSaver(const char* path) : _path(path), _stream(0) {}
		void executeImpl();
		void setStream(Stream* stream) { _stream = stream; }

	private:
		const char* _path;
		Stream* _stream;
	};

	class GameSaverLoader : public Component
	{
	public:
		GameSaverLoader() : _loader("save.dat"), _saver("save.dat") {}
		Hash getType() const { return ComponentsIds::GAME_SAVER_LOADER; }
		virtual Component* clone() const { return 0; }
		void handleMessage(Message& message);

		
	private:

		GameSaver _saver;
		GameLoader _loader;		
	};
}
#endif