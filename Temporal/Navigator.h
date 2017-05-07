#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "StateMachineComponent.h"
#include "Pathfinder.h"

namespace Temporal
{
	class Navigator : public StateMachineComponent
	{
	public:
		Navigator() : StateMachineComponent(getStates(), "NAV"), _destination(Vector::Zero), _path(0){}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Navigator(); }

		const Vector& getDestination() const { return _destination; }
		void setDestination(const Vector& destination) { _destination = destination; }
		Hash getTracked() const { return _tracked; }
		void setTracked(Hash tracked) { _tracked = tracked; }
		NavigationEdgeList* getPath() const { return _path; }
		void setPath(NavigationEdgeList* path) { if(_path) delete _path; _path = path; }
		
		void raiseNavigationSuccess();
		void raiseNavigationFailure();
		bool plotPath();

		static const Hash TYPE;

	protected:
		Hash getInitialState() const;

	private:
		Vector _destination;
		Hash _tracked;
		NavigationEdgeList* _path;

		void postLoad();
		void debugDraw() const;
		HashStateMap getStates() const;

		friend class SerializationAccess;
	};
}
#endif