#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Thread.h"
#include "Hash.h"
#include <unordered_map>

class FTFont;

namespace Temporal
{
	class GameState;
	class SpriteSheet;
	class AnimationSet;
	
	typedef std::unordered_map<Hash, SpriteSheet*> SpriteSheetCollection;
	typedef SpriteSheetCollection::const_iterator SpriteSheetIterator;
	typedef std::unordered_map<Hash, AnimationSet*> AnimationSetCollection;
	typedef AnimationSetCollection::const_iterator AnimationSetIterator;
	typedef std::unordered_map<Hash, FTFont*> FontCollection;
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

		const SpriteSheet* getSpritesheet(Hash id) const { return _spritesheets.at(id); }
		const AnimationSet* getAnimationSet(Hash id) const { return _animationSets.at(id); }
		FTFont* getFont(const char* name, unsigned int size);

	private:
		const char* _gameStateFile;
		bool _isRunning;

		Thread _thread;
		Semaphore _semaphore;

		SpriteSheetCollection _spritesheets;
		AnimationSetCollection _animationSets;
		FontCollection _fonts;

		void initSpritesheets();
		void initAnimationSets();

		ResourceManager() : _gameStateFile(0), _isRunning(false) {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);
	};
}

#endif