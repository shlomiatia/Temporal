#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <Temporal\Base\BaseEnums.h>
#include <vector>

// TODO: Put sprite ,sheet, group in one file SLOTH
namespace Temporal
{
	class Texture;
	class SpriteGroup;

	typedef std::vector<const SpriteGroup*> SpriteGroupCollection;
	typedef SpriteGroupCollection::const_iterator SpriteGroupIterator;

	class SpriteSheet
	{
	public:
		SpriteSheet(const Texture* texture, Orientation::Enum orientation) : _texture(texture), _orientation(orientation) {}
		~SpriteSheet(void);
		void add(const SpriteGroup* element);

		const Texture& getTexture(void) const { return *_texture; }
		Orientation::Enum getOrientation(void) const { return _orientation; }
		const SpriteGroup& get(int spriteGroupID) const;
	private:
		const Texture* _texture;
		const Orientation::Enum _orientation;
		SpriteGroupCollection _spriteGroups;

		SpriteSheet(const SpriteSheet&);
		SpriteSheet& operator=(const SpriteSheet&);
	};
}
#endif