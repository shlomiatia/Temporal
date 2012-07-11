#ifndef SCENENODE_H
#define SCENENODE_H

#include "NumericPair.h"
#include "Hash.h"
#include "BaseEnums.h"
#include <vector>

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	typedef std::vector<SceneNode*> SceneNodeCollection;
	typedef SceneNodeCollection::const_iterator SceneNodeIterator;

	class SceneNode
	{
	public:
		SceneNode(const Hash& id) : _id(id), _translation(Vector::Zero), _spriteGroupId(Hash::INVALID), _spriteId(0) {}
		~SceneNode(void);

		const Hash& getSpriteGroupId(void) const { return _spriteGroupId; }
		void setSpriteGroupId(const Hash& spriteGroupId) { _spriteGroupId = spriteGroupId; }
		int getSpriteId(void) const { return _spriteId; }
		void setSpriteId(int spriteId) { _spriteId = spriteId; }
		const Vector& getTranslation(void) const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }

		SceneNode* get(const Hash& id);
		void draw(const SpriteSheet& spritesheet, Side::Enum orientation);
	private:
		const Hash _id;

		Vector _translation;
		Hash _spriteGroupId;
		int _spriteId;
		SceneNodeCollection _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif