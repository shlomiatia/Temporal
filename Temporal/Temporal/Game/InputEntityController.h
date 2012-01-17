#include "EntityController.h"

namespace Temporal
{
	class InputEntityController : public EntityController
	{
	public:
		InputEntityController(void) {}
		virtual ~InputEntityController(void) {}

		virtual bool isLeft(void) const;
		virtual bool isRight(void) const;
		virtual bool isUp(void) const;
		virtual bool isDown(void) const;
	private:
		InputEntityController(const InputEntityController&) {};
		InputEntityController& operator=(const InputEntityController&) {};
	};
}
