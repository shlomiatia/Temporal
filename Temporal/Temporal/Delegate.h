#ifndef DELEGATE_H
#define DELEGATE_H

namespace Temporal
{
	template <class T>
	class Action
	{
	public:
	   Action(T* obj, void(T::*func)()) : _obj(obj), _func(func) {}

      void operator()() { (*_obj.*_func)(); }

	private:
      void (T::*_func)();
      T* _obj;
	};

	template <class T, class P1>
	class Action1
	{
	public:
		Action1(T* obj, void(T::*func)(P1)) : _obj(obj), _func(func) {}

		void operator()(P1 param) { (*_obj.*_func)(param); }

	private:
		void (T::*_func)(P1);
		T* _obj;
	};
}

#endif