#pragma once
#include <vector>
#include <Temporal\Base\Base.h>
#include "Sprite.h"

namespace Temporal
{
	class SpriteGroup
	{
	public:
		SpriteGroup(void) {}
		~SpriteGroup(void);

		void add(const Sprite* const sprite);

		// TODO: Maybe use sprite iterator instead
		const Sprite& get(unsigned int spriteID) const;
		int getSize(void) const;
	private:
		std::vector<const Sprite* const> _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};
}