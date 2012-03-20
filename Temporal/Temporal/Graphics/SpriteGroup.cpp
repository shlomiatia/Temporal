#include "SpriteGroup.h"
#include "Sprite.h"
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
		return *_sprites[spriteID];
	}

	int SpriteGroup::getSize(void) const
	{
		return _sprites.size();
	}
}