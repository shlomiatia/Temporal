#pragma once

#include <vector>

namespace Temporal
{
	class Sprite;

	class SpriteGroup
	{
	public:
		SpriteGroup(void) {}
		~SpriteGroup(void);

		void add(const Sprite* const sprite);

		// TODO: Maybe use sprite iterator instead SLOTH
		const Sprite& get(int spriteID) const;
		int getSize(void) const;
	private:
		std::vector<const Sprite* const> _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};
}