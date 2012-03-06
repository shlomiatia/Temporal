#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class Body : public Component
	{
	public:
		Body(const Vector& size);

		const Vector& getSize(void) const { return _size; }
		Rect getBounds(void) const;

		virtual void handleMessage(Message& message);

	private:
		Vector _size;
	};
}

