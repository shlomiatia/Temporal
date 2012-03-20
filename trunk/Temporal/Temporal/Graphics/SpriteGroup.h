#ifndef SPRITEGROUP_H
#define SPRITEGROUP_H

#include <vector>

namespace Temporal
{
	class Sprite;

	class SpriteGroup
	{
	public:
		SpriteGroup(void) {}
		~SpriteGroup(void);

		void add(const Sprite* sprite);

		const Sprite& get(int spriteID) const;
		int getSize(void) const;
	private:
		std::vector<const Sprite*> _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};
}
#endif