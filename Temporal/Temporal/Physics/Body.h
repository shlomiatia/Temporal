#pragma once

#include <Temporal\Base\Base.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class Body : public Component
	{
	public:
		Body(const Vector& size);
		virtual ~Body(void) {};

		const Vector& getSize(void) const { return _size; }
		Rect getBounds(void) const;

		virtual void handleMessage(Message& message);

	private:
		Vector _size;

		Body(const Body&);
		Body& operator=(const Body&);
	};
}

