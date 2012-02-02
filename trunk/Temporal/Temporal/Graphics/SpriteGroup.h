#pragma once
#include <vector>
#include <Temporal\Base\Base.h>
#include "Sprite.h"

namespace Temporal
{
	class SpriteGroup
	{
	public:
		// TODO: Use normal data structure
		SpriteGroup(void) {}
		~SpriteGroup(void);
		void add(const Sprite* const sprite);
		const Sprite& get(int spriteID) const;
		int getSize(void) const;
	private:
		std::vector<const Sprite* const> _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};
}