#ifndef DELEGATE_H
#define DELEGATE_H

namespace Temporal
{
	class IAction
	{
	public:
		virtual void operator()() = 0;
	};

	template<class T>
	class Action : public IAction
	{
	public:
		Action(T* obj, void(T::*func)()) : _obj(obj), _func(func) {}

		void operator()() { (*_obj.*_func)(); }

	private:
		void (T::*_func)();
		T* _obj;
	};

	#define createAction(T, func) new Action<T>(this, &T::func)

	template<class P1>
	class IAction1
	{
	public:
		virtual void operator()(P1 param1) = 0;
	};

	template<class T, class P1>
	class Action1 : public IAction1<P1>
	{
	public:
		Action1(T* obj, void(T::*func)(P1)) : _obj(obj), _func(func) {}

		void operator()(P1 param1) { (*_obj.*_func)(param1); }

	private:
		void (T::*_func)(P1);
		T* _obj;
	};

	#define createAction1(T, P1, func) new Action1<T, P1>(this, &T::func)

	template<class R, class P1>
	class IFunc1
	{
	public:
		virtual R operator()(P1 param1) = 0;
	};

	template<class T, class R, class P1>
	class Func1 : public IFunc1<R, P1>
	{
	public:
		Func1(T* obj, R(T::*func)(P1)) : _obj(obj), _func(func) {}

		R operator()(P1 param1) { return (*_obj.*_func)(param1); }

	private:
		R (T::*_func)(P1);
		T* _obj;
	};

#define createFunc1(T, R, P1, func) new Func1<T, R, P1>(this, &T::func)
}

#endif