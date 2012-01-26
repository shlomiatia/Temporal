#include "Body.h"
#include <assert.h>

namespace Temporal
{
	Body::Body(const Vector& position, const Vector& size)
		: _bounds(position, size)
	{
		assert(getBounds().getSize().getX() > 0);
		assert(getBounds().getSize().getY() > 0);
	}
}