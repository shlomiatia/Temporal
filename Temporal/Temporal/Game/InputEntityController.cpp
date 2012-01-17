#include "InputEntityController.h"
#include <Temporal\Input\Input.h>

namespace Temporal
{
	bool InputEntityController::isLeft(void) const { return Input::get().isLeft(); }
	bool InputEntityController::isRight(void) const { return Input::get().isRight(); }
	bool InputEntityController::isUp(void) const { return Input::get().isUp(); }
	bool InputEntityController::isDown(void) const { return Input::get().isDown(); }
}