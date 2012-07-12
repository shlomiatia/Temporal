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
		SceneNode(const Hash& id, bool drawBeforeParent = false) : _id(id), _translation(Vector::Zero), _rotation(0.0f), _spriteGroupID(Hash::INVALID), _spriteInterpolation(0), _drawBeforeParent(drawBeforeParent) {}
		~SceneNode(void);

		const bool drawBeforeParent(void) const { return _drawBeforeParent; }
		const Hash& getID(void) const { return _id; }
		const Hash& getSpriteGroupID(void) const { return _spriteGroupID; }
		void setSpriteGroupID(const Hash& spriteGroupID) { _spriteGroupID = spriteGroupID; }
		float getSpriteInterpolation(void) const { return _spriteInterpolation; }
		void setSpriteInterpolation(float spriteInterpolation) { _spriteInterpolation = spriteInterpolation; }
		const Vector& getTranslation(void) const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		float getRotation(void) const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }

		const SceneNodeCollection& getChildren(void) const { return _children; }

		void add(SceneNode* child) { _children.push_back(child); }
	private:
		const Hash _id;

		bool _drawBeforeParent;
		Vector _translation;
		float _rotation;
		Hash _spriteGroupID;
		float _spriteInterpolation;
		SceneNodeCollection _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif