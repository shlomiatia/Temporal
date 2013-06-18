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
	
	typedef std::unordered_map<Hash, std::shared_ptr<SpriteSheet>> SpriteSheetCollection;
	typedef SpriteSheetCollection::const_iterator SpriteSheetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<AnimationSet>> AnimationSetCollection;
	typedef AnimationSetCollection::const_iterator AnimationSetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<FTFont>> FontCollection;
	typedef FontCollection::const_iterator FontIterator;

	class IOJob
	{
	public:
		virtual void* load() = 0;
		virtual void loaded(void* param) = 0;
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

	class IOAPI
	{
	public:
		static Settings* loadSettings(const char* settingsFile);
		static GameState* loadGameState(const char* gameStateFile);
	private:
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