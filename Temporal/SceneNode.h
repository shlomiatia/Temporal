#ifndef SCENENODE_H
#define SCENENODE_H

#include "Vector.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "Color.h"
#include "Matrix.h"
#include <vector>

namespace Temporal
{
	class SceneNode;
	class SpriteSheet;

	typedef std::vector<SceneNode*> SceneNodeList;
	typedef SceneNodeList::const_iterator SceneNodeIterator;

	class SceneNode
	{
	public:
		explicit SceneNode(Hash id = Hash::INVALID, Hash spriteGroupId = Hash::INVALID, bool drawBehindParent = false, bool transformOnly = false) :
			_id(id), _drawBehindParent(drawBehindParent), _transformOnly(transformOnly), _translation(Vector::Zero), _flip(false), _rotation(0.0f), _center(0.0f), _radius(0.0f),
			_scale(Vector(1.0f, 1.0f)), _spriteGroupId(spriteGroupId), _spriteInterpolation(0.0f), _parent(0) {}
		~SceneNode();

		Hash getID() const { return _id; }

		const bool drawBehindParent() const { return _drawBehindParent; }
		bool isTransformOnly() const { return _transformOnly; }
		
		const Vector& getTranslation() const { return _translation; }
		Matrix getGlobalMatrix() const;
		void setTranslation(const Vector& translation) { _translation = translation; }
		const Vector& getScale() const { return _scale; }
		void setScale(const Vector& scale) { _scale = scale; }
		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }
		bool isFlip() const { return _flip; }
		void setFlip(bool flip) { _flip = flip; }

		Hash getSpriteGroupId() const { return _spriteGroupId; }
		void setSpriteGroupId(Hash spriteGroupId) { _spriteGroupId = spriteGroupId; }
		float getSpriteInterpolation() const { return _spriteInterpolation; }
		void setSpriteInterpolation(float spriteInterpolation) { _spriteInterpolation = spriteInterpolation; }

		float getCenter() const { return _center; }
		float getRadius() const { return _radius; }

		const SceneNodeList& getChildren() const { return _children; }

		void add(SceneNode* child) { _children.push_back(child);}
		SceneNode* clone() const;
		const SceneNode* get(Hash id) const;
		SceneNode* get(Hash id);
		SceneNode& parent() const { return *_parent; }
		void init();

	private:
		Hash _id;
		bool _drawBehindParent;	
		bool _transformOnly;

		Vector _translation;
		float _rotation;
		bool _flip;
		Vector _scale;

		Hash _spriteGroupId;
		float _spriteInterpolation;

		float _center;
		float _radius;

		SceneNode* _parent;
		SceneNodeList _children;

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);

		friend class SerializationAccess;
	};
}
#endif