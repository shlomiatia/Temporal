#pragma once

#include "Rect.h"
#include <assert.h>

namespace Temporal
{
	Rect::Rect(const float centerX, const float centerY, const float getWidth, const float getHeight)
				: _center(centerX, centerY), _size(getWidth, getHeight)
	{
		assert(getSize().getWidth() > 0);
		assert(getSize().getHeight() > 0);
	}
	Rect::Rect(const Vector& center, const Vector& size)
				: _center(center), _size(size)
	{
		assert(getSize().getWidth() > 0);
		assert(getSize().getHeight() > 0);
	}
}