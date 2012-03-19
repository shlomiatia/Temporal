#include "SpriteGroup.h"
#include <assert.h>

namespace Temporal
{
	SpriteGroup::~SpriteGroup(void)
	{
		for(std::vector<const Sprite*>::iterator i = _sprites.begin(); i != _sprites.end(); ++i)
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