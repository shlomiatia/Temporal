#include "SpriteGroup.h"
#include <assert.h>

namespace Temporal
{
	SpriteGroup::~SpriteGroup(void)
	{
		for(std::vector<const Sprite* const>::iterator i = _sprites.begin(); i != _sprites.end(); ++i)
			delete *i;
	}

	void SpriteGroup::add(const Sprite* const sprite)
	{
		_sprites.push_back(sprite);
	}

	const Sprite& SpriteGroup::get(unsigned int spriteID) const
	{
		assert(spriteID <_sprites.size());
		return *_sprites[spriteID];
	}

	int SpriteGroup::getSize(void) const
	{
		return _sprites.size();
	}
}