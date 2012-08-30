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

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("bounds", _bounds);
			serializer.serialize("offset", _offset);
		}
	private:
		AABB _bounds;
		Vector _offset;

		Sprite(const Sprite&);
		Sprite& operator=(const Sprite&);
	};

	typedef std::vector<const Sprite*> SpriteCollection;
	typedef SpriteCollection::const_iterator SpriteIterator;

	class SpriteGroup
	{
	public:
		explicit SpriteGroup(Hash id = Hash::INVALID) : _id(id) {}
		~SpriteGroup();

		Hash getId() const { return _id; }

		void add(const Sprite* sprite);
		const Sprite& get(int spriteID) const;
		int getSize() const;

		template<class T>
		void serialize(T& serializer)
		{
			serializer.serialize("id", _id);
		}
	private:
		Hash _id;
		SpriteCollection _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);
	};

	class Texture;
	typedef std::unordered_map<Hash, const SpriteGroup*> SpriteGroupCollection;
	typedef SpriteGroupCollection::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		explicit SpriteSheet(const Texture* texture = NULL, Side::Enum orientation = Side::RIGHT) : _id(Hash::INVALID), _texture(texture), _orientation(orientation) {}
		~SpriteSheet();
		void add(const SpriteGroup* spriteGroup);

		Hash getId() const { return _id; }
		const Texture& getTexture() const { return *_texture; }
		Side::Enum getOrientation() const { return _orientation; }
		const SpriteGroup& get(Hash spriteGroupID) const;

		void init();

		template<class T>
		void serialize(T& serializer)
		{
			const char* file = NULL;
			serializer.serialize("texture", &file);
			serializer.serialize("orientation", (int&)_orientation);
			_id = Hash(file);
			_texture = Texture::load(file);
		}
	private:
		Hash _id;
		const Texture* _texture;
		Side::Enum _orientation;
		SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}
#endif