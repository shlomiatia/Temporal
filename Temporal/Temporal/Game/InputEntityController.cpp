#include "InputEntityController.h"
#include <Temporal\Input\Input.h>

namespace Temporal
{
	bool InputEntityController::isLeft(void) const { return Input::get().isLeft(); }
	bool InputEntityController::isRight(void) const { return Input::get().isRight(); }
	bool InputEntityController::isUp(void) const { return Input::get().isUp(); }
	bool InputEntityController::isDown(void) const { return Input::get().isDown(); }

	bool CrappyEntityController::isLeft(void) const { return Input::get().isA(); }
	bool CrappyEntityController::isRight(void) const { return Input::get().isD(); }
	bool CrappyEntityController::isUp(void) const { return Input::get().isW(); }
	bool CrappyEntityController::isDown(void) const { return Input::get().isS(); }
}