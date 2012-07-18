#ifndef INPUT_H
#define INPUT_H

#include "NumericPair.h"

namespace Temporal
{
	class Input
	{
	public:
		static Input& get()
		{
			static Input instance;
			return (instance);
		}

		void update();

		bool isLeft() const { return (_left); }
		bool isRight() const { return (_right); }
		bool isUp() const { return (_up); }
		bool isDown() const { return (_down); }
		bool isQuit() const { return (_quit); }
		bool isQ() const { return _q; }
		bool isW() const { return _w; }
		bool isE() const { return _e; }
		const Point& mouse() const { return _mouse; }

	private:
		bool _left;
		bool _right;
		bool _up;
		bool _down;
		bool _quit;
		bool _q;
		bool _w;
		bool _e;
		Point _mouse;

		Input() : _left(false), _right(false), _up(false), _down(false), _quit(false), _q(false), _w(false), _e(false), _mouse(Point::Zero) {}
		~Input() {}
		Input(const Input&);
		Input& operator=(const Input&);
	};
}
#endif