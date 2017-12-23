#include "JumpInfoByTypeProvider.h"
#include "Math.h"

namespace Temporal
{
	static const Hash JUMP_UP_ANIMATION = Hash("POP_ANM_JUMP2");
	static const Hash JUMP_FORWARD_ANIMATION = Hash("POP_ANM_JUMP");
	static const Hash JUMP_FORWARD_END_ANIMATION = Hash("POP_ANM_STAND");

	const JumpInfo JumpInfoByTypeProvider::JUMP_UP_INFO(AngleUtils::radian().ANGLE_90_IN_RADIANS, JUMP_UP_ANIMATION, JUMP_FORWARD_END_ANIMATION);
	const JumpInfo JumpInfoByTypeProvider::JUMP_FORWARD_INFO(AngleUtils::radian().ANGLE_45_IN_RADIANS, JUMP_FORWARD_ANIMATION, JUMP_FORWARD_END_ANIMATION);
}
