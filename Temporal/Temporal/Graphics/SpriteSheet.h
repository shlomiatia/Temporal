#pragma once

#include <vector>
#include <Temporal\Base\Base.h>
#include "Texture.h"
#include "SpriteGroup.h"

namespace Temporal
{
	class SpriteSheet
	{
	public:
		SpriteSheet(const Texture* texture, Orientation::Type orientation) : _texture(texture), _orientation(orientation) {}
		~SpriteSheet(void);
		void add(const SpriteGroup* const element);

		const Texture& getTexture(void) const { return *_texture; }
		Orientation::Type getOrientation(void) const { return _orientation; }
		const SpriteGroup& get(int spriteGroupID) const;
	private:
		const Texture* const _texture;
		std::vector<const SpriteGroup* const> _spriteGroups;
		const Orientation::Type _orientation;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}