#include "ToolsUtils.h"
#include "Keyboard.h"

namespace Temporal
{
	float ToolsUtils::snap(float val, float target, float maxSnap)
	{
		if (Keyboard::get().isKeyPressed(Key::LEFT_SHIFT))
		{
			return val;
		}
		float valModTarget = fmodf(val, target);
		float targetMinusValModTarget = target - valModTarget;
		if (valModTarget <= maxSnap)
			return val - valModTarget;
		else if (targetMinusValModTarget <= maxSnap)
			return val + targetMinusValModTarget;
		else
			return val;
	}

	float ToolsUtils::snap(float val, float target, float maxSnap, float size)
	{
		float snapExtra = target - fmodf(size, target);
		return snap(val + snapExtra, target, maxSnap) - snapExtra;
	}
}