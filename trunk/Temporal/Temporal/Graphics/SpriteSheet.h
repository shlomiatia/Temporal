#pragma once

#include <Temporal\Base\Enums.h>
#include <vector>

namespace Temporal
{
	class Texture;
	class SpriteGroup;

	class SpriteSheet
	{
	public:
		SpriteSheet(const Texture* texture, Orientation::Enum orientation) : _texture(texture), _orientation(orientation) {}
		~SpriteSheet(void);
		void add(const SpriteGroup* const element);

		const Texture& getTexture(void) const { return *_texture; }
		Orientation::Enum getOrientation(void) const { return _orientation; }
		const SpriteGroup& get(int spriteGroupID) const;
	private:
		const Texture* const _texture;
		const Orientation::Enum _orientation;
		std::vector<const SpriteGroup* const> _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}