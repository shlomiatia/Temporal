#include "SpriteSheet.h"
#include "SpriteGroup.h"
#include <assert.h>

namespace Temporal
{
	SpriteSheet::~SpriteSheet(void)
	{
		for(std::vector<const SpriteGroup* const>::iterator i = _spriteGroups.begin(); i != _spriteGroups.end(); ++i)
			delete *i;
	}

	void SpriteSheet::add(const SpriteGroup* const sprite)
	{
		_spriteGroups.push_back(sprite);
	}

	const SpriteGroup& SpriteSheet::get(int spriteGroupID) const
	{
		assert(spriteGroupID < (int)_spriteGroups.size());
		return *_spriteGroups[spriteGroupID];
	}
}