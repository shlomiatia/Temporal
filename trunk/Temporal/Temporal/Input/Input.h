#pragma once

#include <Temporal/Base/Base.h>

namespace Temporal
{
	class Input
	{
	public:
		static Input& get(void)
		{
			static Input instance;
			return (instance);
		}

		void update(void);

		bool isLeft(void) const { return (_left); }
		bool isRight(void) const { return (_right); }
		bool isUp(void) const { return (_up); }
		bool isDown(void) const { return (_down); }
		bool isQuit(void) const { return (_quit); }
		const Vector& mouse(void) const { return _mouse; }

	private:
		bool _left;
		bool _right;
		bool _up;
		bool _down;
		bool _quit;
		Vector _mouse;

		Input(void) : _mouse(Vector::Zero) {}
		~Input(void) {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}