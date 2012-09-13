#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "Hash.h"
#include "BaseEnums.h"
#include "Vector.h"
#include "Shapes.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class Sprite
	{
	public:
		explicit Sprite(const AABB& bounds = AABB::Zero, const Vector& offset = Vector::Zero) 
			: _bounds(bounds), _offset(offset) {}

		const AABB& getBounds() const { return _bounds; }
		const Vector& getOffset() const { return _offset; }
	private:
		AABB _bounds;
		Vector _offset;

		Sprite(const Sprite&);
		Sprite& operator=(const Sprite&);

		friend class SerializationAccess;
	};

	typedef std::vector<Sprite*> SpriteCollection;
	typedef SpriteCollection::const_iterator SpriteIterator;

	class SpriteGroup
	{
	public:
		explicit SpriteGroup(Hash id = Hash::INVALID) : _id(id) {}
		~SpriteGroup();

		Hash getId() const { return _id; }

		void add(Sprite* sprite);
		const Sprite& get(int spriteID) const;
		int getSize() const;

	private:
		Hash _id;
		SpriteCollection _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);

		friend class SerializationAccess;
	};

	class Texture;
	typedef std::unordered_map<Hash, SpriteGroup*> SpriteGroupCollection;
	typedef SpriteGroupCollection::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		explicit SpriteSheet(const Texture* texture = NULL, Side::Enum orientation = Side::RIGHT) : _id(Hash::INVALID), _texture(texture), _orientation(orientation) {}
		~SpriteSheet();
		void add(SpriteGroup* spriteGroup);

		Hash getId() const { return _id; }
		const Texture& getTexture() const { return *_texture; }
		Side::Enum getOrientation() const { return _orientation; }
		const SpriteGroup& get(Hash spriteGroupID) const;

		void init();

	private:
		Hash _id;
		const Texture* _texture;
		Side::Enum _orientation;
		SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);

		friend class SerializationAccess;
	};
}
#endif