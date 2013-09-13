#include "SceneNode.h"
#include "Graphics.h"
#include "SpriteSheet.h"
#include "ResourceManager.h"

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
		SceneNode* clone = new SceneNode(_id, _spriteGroupId, _drawBehindParent, _transformOnly);

		clone->_translation = _translation;
		clone->_rotation = _rotation;
		clone->_scale = _scale;
		
		clone->_spriteInterpolation = _spriteInterpolation;

		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			clone->add((**i).clone());
		}
		return clone;
	}

	const SceneNode* SceneNode::get(Hash id) const
	{
		if(getID() == id)
			return this;
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			const SceneNode* result = (**i).get(id);
			if(result)
				return result;
		}
		return 0;
	}
}