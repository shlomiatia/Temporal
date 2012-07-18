#include "SceneNode.h"
#include "Graphics.h"
#include "SpriteSheet.h"

namespace Temporal
{
	SceneNode::~SceneNode(void)
	{
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			delete *i;
		}
	}

	SceneNode* SceneNode::clone(void) const
	{
		SceneNode* clone = new SceneNode(_id, _drawBeforeParent, _transformOnly);
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			clone->add((**i).clone());
		}
		return clone;
	}
}