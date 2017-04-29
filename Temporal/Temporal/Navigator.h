#ifndef NAVIGATOR_H
#define NAVIGATOR_H
#include "Shapes.h"
#include "StateMachineComponent.h"
#include "Pathfinder.h"

namespace Temporal
{
	class Serialization;

	namespace NavigatorStates
	{
		class Wait : public ComponentState
		{
		public:
			void enter(void* param){};
			void handleMessage(Message& message);
		};

		class Walk : public ComponentState
		{
		public:
			void enter(void* param) { update(); };
			void handleMessage(Message& message);
			void update();
			void updateNext();
		};

		class Fall : public ComponentState
		{
		public:
			Fall() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);

		private:
			bool _afterLoad;
		};

		class JumpUp : public ComponentState
		{
		public:
			JumpUp() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);
		private:
			bool _afterLoad;
		};

		class JumpForward : public ComponentState
		{
		public:
			JumpForward() : _afterLoad(true) {}

			void enter(void* param);
			void handleMessage(Message& message);
		private:
			bool _afterLoad;
		};

		class Descend : public ComponentState
		{
		public:
			Descend() : _afterLoad(false) {}

			void enter(void* param);
			void handleMessage(Message& message);

		private :
			bool _afterLoad;
		};
	}

	class Navigator : public StateMachineComponent
	{
	public:
		Navigator() : StateMachineComponent(getStates(), "NAV"), _destination(OBB::Zero), _path(0) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);

		const OBB& getDestination() const { return _destination; }
		void setDestination(const OBB& destination) { _destination = destination; }
		NavigationEdgeList* getPath() const { return _path; }
		void setPath(NavigationEdgeList* path) { if(_path) delete _path; _path = path; }
		Component* clone() const { return new Navigator(); }

		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:

		OBB _destination;
		NavigationEdgeList* _path;

		void debugDraw() const;

		HashStateMap getStates() const;

		friend class SerializationAccess;
	};
}
#endif