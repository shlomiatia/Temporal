#ifndef PANEL_H
#define PANEL_H

namespace Temporal
{
	class Panel
	{
	public:
		virtual void init() = 0;
		virtual void update(float framePeriodInMillis) = 0;
		virtual void draw() const = 0;
		virtual void dispose() = 0;

		Panel() {};
		virtual ~Panel() {}
	private:
		Panel(const Panel&);
		Panel& operator=(const Panel&);
	};
}

#endif