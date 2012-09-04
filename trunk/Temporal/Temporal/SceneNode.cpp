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
		SceneNode* clone = new SceneNode(_id, _spriteSheetId, _spriteGroupId, _drawBehindParent, _transformOnly);

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

	void SceneNode::init()
	{
		_spriteSheet = ResourceManager::get().getSpritesheet(_spriteSheetId);
		for(SceneNodeIterator i = _children.begin(); i != _children.end(); ++i)
		{
			SceneNode& child = **i;
			if(child._spriteSheetId == Hash::INVALID)
				child._spriteSheetId = _spriteSheetId;
			child.init();
		}
	}
}