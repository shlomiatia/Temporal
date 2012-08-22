#include "SceneNode.h"
#include "Graphics.h"
#include "SpriteSheet.h"

namespace Temporal
{
	SceneNode::~SceneNode()
	{
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			delete *i;
		}
	}

	SceneNode* SceneNode::clone() const
	{
		SceneNode* clone = new SceneNode(_id, _drawBehindParent, _transformOnly);

		clone->setTranslation(getTranslation());
		clone->setScale(getScale());
		clone->setRotation(getRotation());
		clone->setMirrored(isMirrored());

		clone->setSpriteGroupID(getSpriteGroupID());
		clone->setSpriteInterpolation(getSpriteInterpolation());

		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			clone->add((**i).clone());
		}
		return clone;
	}
}