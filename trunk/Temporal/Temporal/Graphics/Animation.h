#pragma once
#include <Temporal\Base\Base.h>
#include "Frame.h"

namespace Temporal
{
	class Animation
	{
	public:
		static const int MAX_ELEMENTS = 20;
		const Frame* _elements[MAX_ELEMENTS];
		int _elementsCount;
		Animation(void) : _elementsCount(0) {}
		~Animation(void) { for(int i = 0; i < _elementsCount; ++i) { delete _elements[i]; } }
		void add(Frame* const element) { _elements[_elementsCount++] = element; }
	private:

		Animation(const Animation&);
		Animation& operator=(const Animation&);
	};
}