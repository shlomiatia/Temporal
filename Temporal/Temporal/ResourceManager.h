#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Hash.h"
#include <unordered_map>

namespace Temporal
{
	class SpriteSheet;

	typedef std::unordered_map<Hash, const SpriteSheet*> SpriteSheetCollection;
	typedef SpriteSheetCollection::const_iterator SpriteSheetIterator;

	class ResourceManager
	{
	public:
		static ResourceManager& get()
		{
			static ResourceManager instance;
			return (instance);
		}

		void init();
		const SpriteSheet* getSpritesheet(const Hash& id);
		void dispose();

	private:
		SpriteSheetCollection _spritesheets;

		ResourceManager() {}
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);
	};
}

#endif