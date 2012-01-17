#pragma once

namespace Temporal
{
	class Panel
	{
	public:
		virtual void init(void) = 0;
		virtual void update(void) = 0;
		virtual void draw(void) = 0;
		virtual void dispose(void) = 0;

		Panel(void) {};
		virtual ~Panel(void) {}
	private:
		Panel(const Panel&);
		Panel& operator=(const Panel&);
	};
}
