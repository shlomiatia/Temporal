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
	class SpriteSheet;
	class AnimationSet;
	class Texture;
	
	typedef std::unordered_map<Hash, std::shared_ptr<SpriteSheet>> SpriteSheetCollection;
	typedef SpriteSheetCollection::const_iterator SpriteSheetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<AnimationSet>> AnimationSetCollection;
	typedef AnimationSetCollection::const_iterator AnimationSetIterator;
	typedef std::unordered_map<Hash, std::shared_ptr<FTFont>> FontCollection;
	typedef FontCollection::const_iterator FontIterator;

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

		void run();

		void queueLoadGameState(const char* gameStateFile);
		GameState* loadGameState(const char* gameStateFile);

		const std::shared_ptr<SpriteSheet> getSpritesheet(const char* file);
		const std::shared_ptr<AnimationSet> getAnimationSet(const char* file);
		const std::shared_ptr<FTFont> getFont(const char* name, unsigned int size);

	private:
		const char* _gameStateFile;
		bool _isRunning;

		Thread _thread;
		Semaphore _semaphore;

		SpriteSheetCollection _spritesheets;
		AnimationSetCollection _animationSets;
		FontCollection _fonts;

		ResourceManager() : _gameStateFile(0), _isRunning(false) {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);
	};
}

#endif