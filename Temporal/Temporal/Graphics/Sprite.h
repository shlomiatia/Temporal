#pragma once

#include <Temporal\Base\Vector.h>
#include <Temporal\Base\Rect.h>

namespace Temporal
{
	class Sprite
	{
	public:
		Sprite(const Rect& bounds, const Vector& offset) 
			: _bounds(bounds), _offset(offset) {}

		const Rect& getBounds(void) const { return _bounds; }
		const Vector& getOffset(void) const { return _offset; }
	private:
		const Rect _bounds;
		const Vector _offset;

		Sprite(const Sprite&);
		Sprite& operator=(const Sprite&);
	};
}