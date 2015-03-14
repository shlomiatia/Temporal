#include "Matrix.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Temporal
{
	void Matrix::rotate(float angle)
	{
		_matrix = glm::rotate(_matrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void Matrix::scale(const Vector& scaleVec)
	{
		_matrix = glm::scale(_matrix, glm::vec3(scaleVec.getX(), scaleVec.getY(), 1.0f));
	}

	void Matrix::translate(const Vector& offsetVec)
	{
		_matrix = glm::translate(_matrix, glm::vec3(offsetVec.getX(), offsetVec.getY(), 0.0f));
	}

	void Matrix::inverse()
	{
		_matrix = glm::inverse(_matrix);
	}
}