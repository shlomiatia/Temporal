#include "SpriteSheet.h"
#include <assert.h>

namespace Temporal
{
	SpriteGroup::~SpriteGroup(void)
	{
		for(SpriteIterator i = _sprites.begin(); i != _sprites.end(); ++i)
			delete *i;
	}

	void SpriteGroup::add(const Sprite* sprite)
	{
		_sprites.push_back(sprite);
	}

	const Sprite& SpriteGroup::get(int spriteID) const
	{
		assert(spriteID < (int)_sprites.size());
		return *_sprites.at(spriteID);
	}

	int SpriteGroup::getSize(void) const
	{
		return _sprites.size();
	}

	SpriteSheet::~SpriteSheet(void)
	{
		delete _texture;
		for(SpriteGroupIterator i = _spriteGroups.begin(); i != _spriteGroups.end(); ++i)
			delete i->second;
	}

	void SpriteSheet::add(const Hash& id, const SpriteGroup* sprite)
	{
		_spriteGroups[id] = sprite;
	}

	const SpriteGroup& SpriteSheet::get(const Hash& spriteGroupID) const
	{
		return *_spriteGroups.at(spriteGroupID);
	}

	const Hash& SpriteSheet::getFirstSpriteGroupID(void) const
	{
		return _spriteGroups.begin()->first;
	}
}