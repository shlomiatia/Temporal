#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Hash.h"
#include <unordered_map>

class FTFont;

namespace Temporal
{
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

		void loadGameState(const char* gameState);

		const SpriteSheet* getSpritesheet(Hash id) const { return _spritesheets.at(id); }
		const AnimationSet* getAnimationSet(Hash id) const { return _animationSets.at(id); }
		FTFont* getFont(const char* name, unsigned int size);

	private:
		SpriteSheetCollection _spritesheets;
		AnimationSetCollection _animationSets;
		FontCollection _fonts;

		void initSpritesheets();
		void initAnimationSets();

		ResourceManager() {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);
	};
}

#endif