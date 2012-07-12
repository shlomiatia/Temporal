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
		SceneNode(const Hash& id) : _id(id), _translation(Vector::Zero), _spriteGroupID(Hash::INVALID), _spriteID(0) {}
		~SceneNode(void);

		const Hash& getSpriteGroupID(void) const { return _spriteGroupID; }
		void setSpriteGroupID(const Hash& spriteGroupID) { _spriteGroupID = spriteGroupID; }
		int getSpriteID(void) const { return _spriteID; }
		void setSpriteId(int spriteId) { _spriteID = spriteId; }
		const Vector& getTranslation(void) const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }

		void add(SceneNode* child) { _children.push_back(child); }
		SceneNode* get(const Hash& id);
		void draw(const SpriteSheet& spritesheet, Side::Enum orientation);
	private:
		const Hash _id;

		Vector _translation;
		Hash _spriteGroupID;
		int _spriteID;
		SceneNodeCollection _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif