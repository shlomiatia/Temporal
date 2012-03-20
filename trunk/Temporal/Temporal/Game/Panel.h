#ifndef PANEL_H
#define PANEL_H

namespace Temporal
{
	class Panel
	{
	public:
		virtual void init(void) = 0;
		virtual void update(float framePeriodInMillis) = 0;
		virtual void draw(void) const = 0;
		virtual void dispose(void) = 0;

		Panel(void) {};
		virtual ~Panel(void) {}
	private:
		Panel(const Panel&);
		Panel& operator=(const Panel&);
	};
}

#endif