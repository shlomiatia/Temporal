#ifndef INPUT_H
#define INPUT_H

#include <Temporal\Base\NumericPair.h>

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
		const Point& mouse(void) const { return _mouse; }

	private:
		bool _left;
		bool _right;
		bool _up;
		bool _down;
		bool _quit;
		Point _mouse; // POINT

		Input(void) : _left(false), _right(false), _up(false), _down(false), _quit(false), _mouse(Point::Zero) {}
		~Input(void) {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}
#endif