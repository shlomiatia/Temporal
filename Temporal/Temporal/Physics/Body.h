#pragma once

#include <Temporal\Base\Base.h>

namespace Temporal
{
	typedef class Body StaticBody;

	class Body
	{
	public:
		Body(const Vector& position, const Vector& size);
		virtual ~Body(void) {};

		const Vector& getPosition(void) const { return _bounds.getCenter(); }
		const Rect& getBounds(void) const { return _bounds; }

	protected:
		Rect _bounds;

	private:
		Body(const Body&);
		Body& operator=(const Body&);
	};
}

