#include "SceneNode.h"

namespace Temporal
{
	SceneNode::~SceneNode(void)
	{
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			delete *i;
		}
	}

	void SceneNode::draw(void)
	{
	}
}