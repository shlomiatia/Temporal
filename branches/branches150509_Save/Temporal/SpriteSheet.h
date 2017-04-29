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

	typedef std::vector<Sprite*> SpriteList;
	typedef SpriteList::const_iterator SpriteIterator;

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
		SpriteList _sprites;

		SpriteGroup(const SpriteGroup&);
		SpriteGroup& operator=(const SpriteGroup&);

		friend class SerializationAccess;
	};

	class Texture;
	typedef std::unordered_map<Hash, SpriteGroup*> HashSpriteGroupMap;
	typedef HashSpriteGroupMap::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		explicit SpriteSheet(char* textureFile);
		explicit SpriteSheet(const Texture* texture = 0, Side::Enum orientation = Side::RIGHT) :
			_textureFile(), _texture(texture), _orientation(orientation) {}
		~SpriteSheet();
		void add(SpriteGroup* spriteGroup);

		const Texture& getTexture() const { return *_texture; }
		Side::Enum getOrientation() const { return _orientation; }
		const SpriteGroup& get(Hash spriteGroupID) const;

		void init();
	private:
		std::string _textureFile;
		const Texture* _texture;
		Side::Enum _orientation;
		HashSpriteGroupMap _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);

		friend class SerializationAccess;
	};
}
#endif