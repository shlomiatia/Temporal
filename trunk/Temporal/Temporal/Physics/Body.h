#ifndef BODY_H
#define BODY_H

#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Rect.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class Body : public Component
	{
	public:
		explicit Body(const Size& size);
		virtual ~Body(void) {}

		const Size& getSize(void) const { return _size; }
		Rect getBounds(void) const;

		virtual void handleMessage(Message& message);

	private:
		Size _size;
	};
}


#endif