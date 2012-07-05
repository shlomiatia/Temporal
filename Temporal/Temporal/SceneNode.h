#ifndef SCENENODE_H
#define SCENENODE_H

#include "NumericPair.h"
#include "Hash.h"
#include <vector>

namespace Temporal
{
	class SceneNode;

	typedef std::vector<SceneNode*> SceneNodeCollection;
	typedef SceneNodeCollection::const_iterator SceneNodeIterator;

	class SceneNode
	{
	public:
		SceneNode(const Hash& id) : _id(id), _translation(Vector::Zero), _rotation(0.0f), _spriteGroupId(Hash::INVALID), _spriteId(0) {}
		~SceneNode(void);

		const Hash& getSpriteGroupId(void) const { return _spriteGroupId; }
		int getSpriteId(void) const { return _spriteId; }
		void setSpriteGroupId(const Hash& spriteGroupId) { _spriteGroupId = spriteGroupId; }
		void setSpriteId(int spriteId) { _spriteId = spriteId; }

		void draw(void);
	private:
		const Hash _id;

		Vector _translation;
		float _rotation;
		Hash _spriteGroupId;
		int _spriteId;
		SceneNodeCollection _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif