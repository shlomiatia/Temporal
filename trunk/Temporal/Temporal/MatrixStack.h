#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include "Vector.h"
#include <glm/glm.hpp>
#include <stack>

namespace Temporal
{
	class MatrixStack
	{
	public:
		MatrixStack() : _matrix() {}

		const glm::mat4& top() const { return _matrix; }

		void reset() { _matrix = glm::mat4(); }

		void rotate(float angle);
		void scale(const Vector& scaleVec);
		void translate(const Vector& offsetVec);

		void push()	{ _matrices.push(_matrix);	}
		void pop() { _matrix = _matrices.top(); _matrices.pop(); }

	private:
		glm::mat4 _matrix;
		std::stack<glm::mat4> _matrices;
	};

}

#endif