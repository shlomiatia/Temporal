#pragma once
#include <Temporal\Base\Base.h>

namespace Temporal
{
	class Frame
	{
	public:
		Frame(const Rect& bounds, const Vector& offset) 
			: _bounds(bounds), _offset(offset) {}

		const Rect& getBounds(void) const { return _bounds; }
		const Vector& getOffset(void) const { return _offset; }
	private:
		const Rect _bounds;
		const Vector _offset;

		Frame(const Frame&);
		Frame& operator=(const Frame&);
	};
}