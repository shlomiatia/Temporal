#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Thread.h"
#include "Hash.h"
#include <unordered_map>
#include <memory>
#include <string>

class FTFont;

namespace Temporal
{
	class GameState;
	class Settings;
	class SpriteSheet;
	class AnimationSet;
	class Font;
	class Texture;
	class Stream;
	
	typedef std::unordered_map<Hash, std::shared_ptr<SpriteSheet>> HashSpriteSheetMap;
	typedef HashSpriteSheetMap::const_iterator SpriteSheetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<AnimationSet>> HashAnimationSetMap;
	typedef HashAnimationSetMap::const_iterator AnimationSetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<Font>> HashFontMap;
	typedef HashFontMap::const_iterator FontIterator;

	typedef std::vector<std::string> StringList;
	typedef StringList::const_iterator StringIterator;
	typedef std::unordered_map<Hash, GameState*> HashGameStateMap;
	typedef HashGameStateMap::const_iterator GameStateIterator;

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
		const HashGameStateMap& getResult() const { return _result; }

	private:
		StringList _files;
		HashGameStateMap _result;
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
		const std::shared_ptr<SpriteSheet> getSingleTextureSpritesheet(const char* file);
		const std::shared_ptr<AnimationSet> getAnimationSet(const char* file);
		const std::shared_ptr<Font> getFont(const char* name, unsigned int size);

	private:
		HashSpriteSheetMap _spritesheets;
		HashAnimationSetMap _animationSets;
		HashFontMap _fonts;

		ResourceManager() {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);

		friend class AnimationEditor;
	};
}

#endif