#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"
#include <glm/glm.hpp>
#include <stack>

namespace Temporal
{
	class Matrix
	{
	public:
		Matrix() {}

		void reset() { _matrix = glm::mat4(); }
		void resetTranslation() { _matrix[3] = glm::vec4(0.0f, 0.0f, 0.f, 1.0f); }

		void rotate(float angle);
		void scale(const Vector& scaleVec);
		void translate(const Vector& offsetVec);
		void inverse();

		Vector operator*(const Vector& vector) const
		{
			glm::vec4 vec = _matrix * glm::vec4(vector.getX(), vector.getY(), 0.0f, 1.0f);
			return Vector(vec.x, vec.y);
		}

		Matrix operator*(const Matrix& other) const
		{
			glm::mat4 mat = _matrix * other._matrix;
			return Matrix(mat);
		}

		Matrix& operator*=(const Matrix& other)
		{
			_matrix *= other._matrix;
			return *this;
		}
		
	private:
		glm::mat4 _matrix;

		explicit Matrix(glm::mat4 matrix) : _matrix(matrix) {}
	};

	class MatrixStack
	{
	public:
		MatrixStack() : _matrix() {}

		Matrix& top() { return _matrix; }

		void push()	{ _matrices.push(_matrix);	}
		void pop() { _matrix = _matrices.top(); _matrices.pop(); }

	private:
		Matrix _matrix;
		std::stack<Matrix> _matrices;
	};

}

#endif