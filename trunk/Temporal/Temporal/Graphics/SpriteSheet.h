#pragma once
#include <Temporal\Base\Base.h>
#include "Texture.h"
#include "Animation.h"

namespace Temporal
{
	class SpriteSheet
	{
	public:
		// TODO: Dynamic
		static const int MAX_ELEMENTS = 10;
		Animation* _elements[MAX_ELEMENTS];
		int _elementsCount;
		SpriteSheet(const Texture* const texture) 
			: _texture(texture), _elementsCount(0) {}
		~SpriteSheet(void) { for(int i = 0; i < _elementsCount; ++i) { delete _elements[i]; } delete _texture; }
		void add(Animation* element) { _elements[_elementsCount++] = element; }

		const Texture& getTexture(void) const { return *_texture; }
	private:
		const Texture* const _texture;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}