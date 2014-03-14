#include "MatrixStack.h"
#include "Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Temporal
{
	void MatrixStack::rotate(float angle)
	{
		float radians = toRadians(angle);
		_matrix = glm::rotate(_matrix, radians, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void MatrixStack::scale(const Vector& scaleVec)
	{
		_matrix = glm::scale(_matrix, glm::vec3(scaleVec.getX(), scaleVec.getY(), 0.0f));
	}

	void MatrixStack::translate(const Vector& offsetVec)
	{
		_matrix = glm::translate(_matrix, glm::vec3(offsetVec.getX(), offsetVec.getY(), 0.0f));
	}
}