#pragma once
#include <Temporal\Base\Base.h>
#include "Sprite.h"

namespace Temporal
{
	class SpriteGroup
	{
	public:
		// TODO: Use normal data structure
		static const int MAX_ELEMENTS = 20;
		const Sprite* _elements[MAX_ELEMENTS];
		int _elementsCount;
		SpriteGroup(void) : _elementsCount(0) {}
		~SpriteGroup(void) { for(int i = 0; i < _elementsCount; ++i) { delete _elements[i]; } }
		void add(Sprite* const element) { _elements[_elementsCount++] = element; }
	private:

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};
}