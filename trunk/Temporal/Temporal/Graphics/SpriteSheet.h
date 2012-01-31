#pragma once
#include <Temporal\Base\Base.h>
#include "Texture.h"
#include "SpriteGroup.h"

namespace Temporal
{
	class SpriteSheet
	{
	public:
		static const int MAX_ELEMENTS = 20;
		SpriteGroup* _elements[MAX_ELEMENTS];
		int _elementsCount;
		SpriteSheet(const Texture* texture) 
			: _texture(texture), _elementsCount(0) {}
		~SpriteSheet(void) { for(int i = 0; i < _elementsCount; ++i) { delete _elements[i]; } delete _texture; }
		void add(SpriteGroup* const element) { _elements[_elementsCount++] = element; }

		const Texture& getTexture(void) const { return *_texture; }
	private:
		const Texture* const _texture;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}