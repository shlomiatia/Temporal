#include "SpriteSheet.h"
#include "Texture.h"
#include <assert.h>

namespace Temporal
{
	SpriteGroup::~SpriteGroup()
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
		assert(static_cast<unsigned int>(spriteID) < _sprites.size());
		return *_sprites.at(spriteID);
	}

	int SpriteGroup::getSize() const
	{
		return _sprites.size();
	}

	SpriteSheet::~SpriteSheet()
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

	const Hash& SpriteSheet::getFirstSpriteGroupID() const
	{
		return _spriteGroups.begin()->first;
	}
}