#include "MovementUtils.h"
#include <math.h>

namespace Temporal
{
	float getJumpHeight(float angle, float jumpForce, float gravity, float distance)
	{
		/* x = T*F*cos(A)
		 * y = T*F*sin(A) + (G*T^2)/2
		 *
		 * T = x/(F*cos(A))
		 * y = (x/(F*cos(A))*F*sin(A) + (G*(x/(F*cos(A)))^2)/2
		 * y = (x*sin(A))/(cos(A)) + (G*x^2)/(2*F^2*cos(A)^2)
		 * y = (2*F^2*x*sin(A)*cos(A) + G*x^2)/(2*F^2*cos(A)^2)
		 * y = x*(2*F^2*sin(A)*cos(A) + G*x)/(2*F^2*cos(A)^2)
		 */
		float height = distance*(2.0f*pow(jumpForce,2.0f)*sin(angle)*cos(angle) + gravity*distance)/(2.0f*pow(jumpForce,2.0f)*pow(cos(angle),2.0f));
		return height;
	}

	float getMaxJumpHeight(float angle, float jumpForce, float gravity)
	{
		/* v = F + G*T
		 * p = F*T + (G*T^2)/2 
		 *
		 * 0 = F + G*T
		 * G*T = -F
		 * T = -F/G
		 *
		 * y = F*(-F/G) + (G*(-F/G)^2)/2
		 * y = -(F^2)/G + (G*F^2)/(2*G^2)
		 * y = -(F^2)/G + (F^2)/(2*G)
		 * y = (-2*F^2 + F^2)/(2*G)
		 * y = (-F^2)/(2*G)
		 */

		float maxHeight = -pow(jumpForce*sin(angle), 2.0f) / (2*gravity);
		return maxHeight;
	}

	float getMaxJumpDistance(float angle, float jumpForce, float gravity)
	{
		/* v = F + G*T
		 * p = F*T + (G*T^2)/2 
		 *
		 * -F = F + G*T
		 * G*T = -2*F
		 * T = -2*F/G
		 *
		 * x = F*T
		 * x = F*(-2*F/G)
		 * x = -(2*F^2)/G
		 */
		float maxDistance = -2*pow(jumpForce*cos(angle), 2.0f) / (gravity);
		return maxDistance;
	}

	float getFallDistance(float fallStartForce, float gravity, float height)
	{
		/* x = T*F
		 * y = -(G*T^2)/2
		 *
		 * T = sqrt(-(2*y)/G))
		 * x = sqrt(-(2*y)/G))*F
		 */
		float fallDistance = sqrt((2*height)/gravity)*fallStartForce;
		return fallDistance;
	}
}