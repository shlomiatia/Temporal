#pragma once

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

	private:
		bool _left;
		bool _right;
		bool _up;
		bool _down;
		bool _quit;

		Input(void) {}
		~Input(void) {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}