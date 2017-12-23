#ifndef FONT_H
#define FONT_H

#include "Shapes.h"
#include "Color.h"
#include "ft2build.h"
#include "freetype.h"

#include <unordered_map>

namespace Temporal
{
	class Glyph
	{
	public:
		explicit Glyph(const AABB& bounds = AABB::Zero, const Vector& bearing = Vector::Zero, const Vector& advance = Vector::Zero) 
			: _bounds(bounds), _bearing(bearing), _advance(advance) {}

		const AABB& getBounds() const { return _bounds; }
		const Vector& getBearing() const { return _bearing; }
		const Vector& getAdvance() const { return _advance; }
	private:
		AABB _bounds;
		Vector _bearing;
		Vector _advance;
	};

	class Texture;

	class Font
	{
	public:
		Font(const char* name, int size);
		~Font();

		void draw(const char* string, const Vector& position = Vector::Zero, const Color& color = Color::White, bool ignoreLastCharForWidth = false);
	private:
		float _newLineAdvance;
		float _spaceAdvance;

		const Texture* _texture;
		std::unordered_map<char, Glyph> _glyphs;

		Font(const Font&);
		Font& operator=(const Font&);
	};

	class FontManager
	{
	public:
		static FontManager& get()
		{
			static FontManager instance;
			return (instance);
		}
		const FT_Library& getFTLib() const { return _ftLib; }

		void init();
		void dispose();
	private:
		FT_Library _ftLib;

		FontManager() {};
		FontManager(const FontManager&);
		FontManager& operator=(const FontManager&);
	};
}
#endif