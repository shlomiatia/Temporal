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
		SceneNode(const Hash& id, bool drawBehindParent = false, bool transformOnly = false, const Hash& spriteGroupID = Hash::INVALID, const Vector& translation = Vector::Zero, float rotation = 0.0f, bool isMirrored = false) :
		  _id(id), _translation(translation), _rotation(rotation), _isMirrored(isMirrored), _spriteGroupID(spriteGroupID), _spriteInterpolation(0),
		  _drawBehindParent(drawBehindParent), _transformOnly(transformOnly) {}
		~SceneNode();

		const bool drawBehindParent() const { return _drawBehindParent; }
		const Hash& getID() const { return _id; }
		const Hash& getSpriteGroupID() const { return _spriteGroupID; }
		void setSpriteGroupID(const Hash& spriteGroupID) { _spriteGroupID = spriteGroupID; }
		float getSpriteInterpolation() const { return _spriteInterpolation; }
		void setSpriteInterpolation(float spriteInterpolation) { _spriteInterpolation = spriteInterpolation; }
		const Vector& getTranslation() const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }
		bool isMirrored() const { return _isMirrored; }
		void setMirrored(bool isMirrored) { _isMirrored = isMirrored; }
		bool isTransformOnly() const { return _transformOnly; }
		const SceneNodeCollection& getChildren() const { return _children; }

		void add(SceneNode* child) { _children.push_back(child); }
		SceneNode* clone() const;
	private:
		const Hash _id;

		bool _drawBehindParent;		
		bool _transformOnly;
		Vector _translation;
		float _rotation;
		bool _isMirrored;

		Hash _spriteGroupID;
		float _spriteInterpolation;

		SceneNodeCollection _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif