#include "NavigationNode.h"

namespace Temporal
{
	NavigationNode::~NavigationNode()
	{
		for (NavigationEdgeIterator i = _edges.begin(); i != _edges.end(); ++i)
		{
			delete *i;
		}
	}
}