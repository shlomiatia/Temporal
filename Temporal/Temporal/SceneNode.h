#ifndef SCENENODE_H
#define SCENENODE_H

#include "Vector.h"
#include "Hash.h"
#include "BaseEnums.h"
#include "Color.h"
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
		explicit SceneNode(Hash id = Hash::INVALID, Hash spriteSheetId = Hash::INVALID, Hash spriteGroupId = Hash::INVALID, bool drawBehindParent = false,
						   bool transformOnly = false) : _id(id), _drawBehindParent(drawBehindParent), _transformOnly(transformOnly), _translation(Vector::Zero),
						   _rotation(0.0f), _isMirrored(false), _spriteSheetId(spriteSheetId), _spriteGroupId(spriteGroupId), _spriteInterpolation(0.0f) 
			{ init(); }
		~SceneNode();

		Hash getID() const { return _id; }

		const bool drawBehindParent() const { return _drawBehindParent; }
		bool isTransformOnly() const { return _transformOnly; }
		
		const Vector& getTranslation() const { return _translation; }
		void setTranslation(const Vector& translation) { _translation = translation; }
		bool isMirrored() const { return _isMirrored; }
		void setMirrored(bool isMirrored) { _isMirrored = isMirrored; }
		float getRotation() const { return _rotation; }
		void setRotation(float rotation) { _rotation = rotation; }

		const SpriteSheet& getSpriteSheet() const { return *_spriteSheet; }
		Hash getSpriteSheetId() const { return _spriteSheetId; }
		Hash getSpriteGroupId() const { return _spriteGroupId; }
		void setSpriteGroupId(Hash spriteGroupId) { _spriteGroupId = spriteGroupId; }
		float getSpriteInterpolation() const { return _spriteInterpolation; }
		void setSpriteInterpolation(float spriteInterpolation) { _spriteInterpolation = spriteInterpolation; }

		const SceneNodeCollection& getChildren() const { return _children; }

		void add(SceneNode* child) { _children.push_back(child); }
		SceneNode* clone() const;

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("sprite-sheet", _spriteSheetId);
			serializer.serialize("sprite-group", _spriteGroupId);
			init();
		}
	private:
		Hash _id;

		bool _drawBehindParent;	
		bool _transformOnly;

		Vector _translation;
		float _rotation;
		bool _isMirrored;

		const SpriteSheet* _spriteSheet;
		Hash _spriteSheetId;
		Hash _spriteGroupId;
		float _spriteInterpolation;

		SceneNodeCollection _children;

		void init();

		SceneNode(const SceneNode&);
		SceneNode& operator=(const SceneNode&);
	};
}
#endif