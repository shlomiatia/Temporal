#ifndef STATICBODY_H
#define STATICBODY_H

#include <Temporal\Base\Segment.h>
#include <Temporal\Game\Component.h>

namespace Temporal
{
	class StaticBody : public Component
	{
	public:
		StaticBody(const Segment& segment, bool cover) : _segment(segment), _cover(cover) {};

		bool isCover(void) const { return _cover; }
		const Segment& getSegment(void) const { return _segment; }
		ComponentType::Enum getType(void) const { return ComponentType::STATIC_BODY; }
		void handleMessage(Message& message);

	private:
		bool _cover;
		Segment _segment;
	};
}
#endif