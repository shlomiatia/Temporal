#ifndef MOVEMENTUTILS_H
#define MOVEMENTUTILS_H

namespace Temporal
{
	float getJumpHeight(float angle, float jumpForce, float gravity, float distance);
	float getMaxJumpHeight(float angle, float jumpForce, float gravity);
	float getMaxJumpDistance(float angle, float jumpForce, float gravity);
	float getFallDistance(float forcePerSecond, float gravity, float height);
}

#endif