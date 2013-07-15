#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Thread.h"
#include "Hash.h"
#include <unordered_map>
#include <memory>

class FTFont;

namespace Temporal
{
	class GameState;
	class Settings;
	class SpriteSheet;
	class AnimationSet;
	class Texture;
	class Stream;
	
	typedef std::unordered_map<Hash, std::shared_ptr<SpriteSheet>> SpriteSheetCollection;
	typedef SpriteSheetCollection::const_iterator SpriteSheetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<AnimationSet>> AnimationSetCollection;
	typedef AnimationSetCollection::const_iterator AnimationSetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<FTFont>> FontCollection;
	typedef FontCollection::const_iterator FontIterator;

	typedef std::vector<std::string> StringCollection;
	typedef StringCollection::const_iterator StringIterator;
	typedef std::vector<GameState*> GameStateCollection;
	typedef GameStateCollection::const_iterator GameStateIterator;

	class IOJob
	{
	public:
		IOJob() : _isStarted(false), _isFinished(false) {}
		virtual ~IOJob() {}

		bool isStarted() const { return _isStarted; }
		void setStarted() { _isStarted = true; }
		bool isFinished() const { return _isFinished; }

		void execute() { executeImpl(); _isFinished = true; } 
		void reset() { _isStarted = false; _isFinished = false; }

	protected:
		virtual void executeImpl() = 0;
	private:
		bool _isStarted;
		bool _isFinished;

		IOJob(const IOJob&);
		IOJob& operator=(const IOJob&);
	};

	class IOThread
	{
	public:
		static IOThread& get()
		{
			static IOThread instance;
			return (instance);
		}

		void init();
		void dispose();

		void setJob(IOJob* job);

		void run();

	private:
		bool _isRunning;

		Thread _thread;
		Semaphore _semaphore;
		IOJob* _job;

		IOThread() : _isRunning(false) {}
		IOThread(const IOThread&);
		IOThread& operator=(const IOThread&);
	};

	typedef std::unordered_map<std::string, std::string> StringMap;
	typedef StringMap::const_iterator StringMapIterator;

	class SettingsLoader : public IOJob
	{
	public:
		SettingsLoader(const char* path) : _path(path) {};
		void executeImpl();

		Settings* getResult() { return _result; }

	private:
		const char* _path;
		Settings* _result;
	};

	class GameStateLoader : public IOJob
	{
	public:
		GameStateLoader(const char* file = 0);
		void executeImpl();

		void add(const char* file);
		const StringCollection& getFiles() const { return _files; }
		const GameStateCollection& getResult() const { return _result; }

	private:
		StringCollection _files;
		GameStateCollection _result;
	};

	class ResourceManager
	{
	public:
		static ResourceManager& get()
		{
			static ResourceManager instance;
			return (instance);
		}

		void init();
		void dispose();
		
		void collectGarbage();

		const std::shared_ptr<SpriteSheet> getSpritesheet(const char* file);
		const std::shared_ptr<SpriteSheet> getTexture(const char* file);
		const std::shared_ptr<AnimationSet> getAnimationSet(const char* file);
		const std::shared_ptr<FTFont> getFont(const char* name, unsigned int size);

	private:
		
		SpriteSheetCollection _spritesheets;
		AnimationSetCollection _animationSets;
		FontCollection _fonts;

		ResourceManager() {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);
	};
}

#endif