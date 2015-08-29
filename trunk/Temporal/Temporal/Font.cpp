#include "Font.h"
#include "Texture.h"
#include "Graphics.h"
#include "ftglyph.h"

#include <algorithm>
#include <GL/glew.h>

namespace Temporal
{
	Font::Font(const char* name, int size) : _newLineAdvance(0.0f), _spaceAdvance(0.0f), _texture(0)
	{
		const FT_Library& ftLib = FontManager::get().getFTLib();
		FT_Face face;
		FT_Error error = FT_New_Face(ftLib, name, 0, &face);
		if(error != 0)
			abort();

		error = FT_Set_Char_Size(face, size << 6, size << 6, 96, 96);
		if(error != 0)
			abort();

		Vector textureSize = Vector::Zero;
		for(char i = '!'; i <= '~'; ++i)
		{
			error = FT_Load_Char(face, i, FT_LOAD_RENDER);
			if(error != 0)
				abort();

			textureSize.setX(textureSize.getX() + static_cast<float>(face->glyph->bitmap.width) + 1.0f);
			textureSize.setY(std::max(textureSize.getY(), static_cast<float>(face->glyph->bitmap.rows)));
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		_texture = Texture::load(textureSize, true);

		int x = 0;
		int j = 0;
		for(char i = '!'; i <= '~'; ++i)
		{
			FT_Load_Char(face, i, FT_LOAD_RENDER);
			if(error != 0)
				abort();
			
			FT_GlyphSlot slot = face->glyph;
			FT_Bitmap& bitmap = slot->bitmap;

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
			
			AABB texturePart(x + bitmap.width/2.0f, bitmap.rows/2.0f, bitmap.width, bitmap.rows);
			float bearingX = slot->metrics.horiBearingX >> 6;
			float bearingY = slot->metrics.horiBearingY >> 6;
			Vector bearing(bearingX, bearingY);
			float advanceX = static_cast<float>(slot->advance.x >> 6);
			float advanceY = static_cast<float>(slot->advance.y >> 6);
			Vector advance(advanceX, advanceY);
			
			
			x += slot->bitmap.width + 1;
			Glyph glyph(texturePart, bearing, advance);
			float newLineAdvance = 2 * bitmap.rows - bearingY;
			if(newLineAdvance, _newLineAdvance)
				_newLineAdvance = newLineAdvance;
			float spaceAdvance = bitmap.width - bearingX;
			++j;
			_spaceAdvance = _spaceAdvance + (spaceAdvance - _spaceAdvance) / j;
			_glyphs[i] = glyph;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		error = FT_Done_Face(face);
		if(error != 0)
			abort();
	}

	Font::~Font()
	{
		delete _texture;
	}

	void Font::draw(const char* string, const Vector& position, const Color& color)
	{
		Vector currentPosition(position);
		float width = 0.0f;
		float height = 0.0f;
		for(int i = 0; ; ++i)
		{
			char c = string[i];
			if(!c)
				break;
			if(c == ' ')
			{
				width += _spaceAdvance;
				continue;
			}
			const Glyph& glyph = _glyphs[c];
			width += glyph.getAdvance().getX();
			height = std::max(height, glyph.getBearing().getY());
		}
		currentPosition.setX(currentPosition.getX() - width / 2.0f);
		currentPosition.setY(currentPosition.getY() - height / 2.0f);
		for(int i = 0; ; ++i)
		{
			char c = string[i];
			if(!c)
				break;
			if(c == ' ')
			{
				currentPosition.setX(currentPosition.getX() + _spaceAdvance);
				continue;
			}
			/*if(c == '\n')
			{
				currentPosition.setX(position.getX());
				currentPosition.setY(currentPosition.getY() - _newLineAdvance);
				continue;
			}*/

			const Glyph& glyph = _glyphs[c];
			
			Vector translation = currentPosition + glyph.getBounds().getRadius();
			translation.setX(translation.getX() + glyph.getBearing().getX());
			translation.setY(translation.getY() + glyph.getBearing().getY() - glyph.getBounds().getHeight());
			Graphics::get().getSpriteBatch().add(_texture,  translation, glyph.getBounds(), color);
			currentPosition += glyph.getAdvance();
		}
	}

	void FontManager::init() 
	{ 
		FT_Error error = FT_Init_FreeType(&_ftLib);
		if(error != 0)
			abort(); 
	}

	void FontManager::dispose()
	{ 
		FT_Done_FreeType(_ftLib); 
	}
}