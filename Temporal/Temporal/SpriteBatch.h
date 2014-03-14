#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Vector.h"
#include "Shapes.h"
#include "Color.h"
#include "Math.h"
#include <vector>

namespace Temporal
{
	class Texture;

	class SpriteBatchItem
	{
	public:
		SpriteBatchItem() : texture(0), rotation(0.0f), color(Color::White) {}

		const Texture* texture;
		Vector objectTranslation;
		float rotation;
		Vector rotationTranslation;
		Vector radius;
		Color color;
		AABB texturePart;

		unsigned int getTextureId() const;
	};

	namespace SpriteBatchMode
	{
		enum Enum
		{
			LINES,
			TRIANGLES
		};
	}

	typedef std::vector<SpriteBatchItem> SpriteBatchItemCollection;
	typedef SpriteBatchItemCollection::const_iterator SpriteBatchItemIterator;

	class SpriteBatch
	{
	public:

		SpriteBatch() : _mode(SpriteBatchMode::TRIANGLES), _size(0), _vertices(0), _vao(0), _vbo(0), _ibo(0), _lastTexture(0), 
			_coordinateAttribute(0), _textureCoordinateAttribute(0), _colorAttribute(0), _textureUniform(0),
			_transformUniform(0), _typeUniform(0) {}
		~SpriteBatch();

		void init();
		void begin();

		// Partial pivoted texture
		void add(const Texture* texture, const Vector& objectTranslation, const AABB& texturePart = AABB(0.0f, 0.0f, 1.0f, 1.0f), 
			float rotation = 0.0f, const Vector& rotationTranslation = Vector::Zero, const Vector& radius = Vector::Zero, const Color& color = Color::White);
		// Partial texture
		void add(const Texture* texture, const Vector& objectTranslation, const AABB& texturePart = AABB(0.0f, 0.0f, 1.0f, 1.0f), 
			float rotation = 0.0f, const Vector& radius = Vector::Zero, const Color& color = Color::White)
		{
			add(texture, objectTranslation, texturePart, rotation, Vector::Zero, radius, color);
		}
		// Full pivoted texture
		void add(const Texture* texture, const Vector& objectTranslation, float rotation = 0.0f, const Vector& rotationTranslation = Vector::Zero,
			const Vector& radius = Vector::Zero, const Color& color = Color::White)
		{
			add(texture, objectTranslation, AABB(0.0f, 0.0f, 1.0f, 1.0f), rotation, rotationTranslation, radius, color);
		}
		// Full texture
		void add(const Texture* texture, const Vector& objectTranslation, float rotation = 0.0f, const Vector& radius = Vector::Zero, const Color& color = Color::White)
		{
			add(texture, objectTranslation, AABB(0.0f, 0.0f, 1.0f, 1.0f), rotation, Vector::Zero, radius, color);
		}
		// OBB Shape
		void add(const Vector& objectTranslation, const Vector& radius, float rotation = 0.0f, const Color& color = Color::White)
		{
			add(0, objectTranslation, AABB(0.0f, 0.0f, 1.0f, 1.0f), fromRadians(rotation), Vector::Zero, radius, color);
		}
		void add(const OBB& obb, const Color& color = Color::White)
		{
			add(obb.getCenter(), obb.getRadius(), obb.getAngle(), color);
		}
		// AABB Shape
		void add(const Vector& objectTranslation, const Vector& radius, const Color& color = Color::White)
		{
			add(0, objectTranslation, AABB(0.0f, 0.0f, 1.0f, 1.0f), 0.0f, Vector::Zero, radius, color);
		}
		void add(const AABB& abb, const Color& color = Color::White)
		{
			add(abb.getCenter(), abb.getRadius(), color);
		}
		void end();

		void setMode(SpriteBatchMode::Enum mode) { _mode = mode; }
	
	private:
		static const int INITLAL_MAX_SPRITES = 256;

		SpriteBatchMode::Enum _mode;
		
		std::vector<SpriteBatchItem> _items;
		int _size;
		float* _vertices;
		unsigned int _vao, _vbo, _ibo;
		
		unsigned int _lastTexture;

		int _coordinateAttribute, _textureCoordinateAttribute, _colorAttribute;
		int _textureUniform, _transformUniform, _typeUniform;

		int getCoordinateAttribute() const { return _coordinateAttribute; }
		int getTextureCoordinateAttribute() const { return _textureCoordinateAttribute; }
		int getColorAttribute() const { return _colorAttribute; }
		int getTextureUniform() const { return _textureUniform; }
		int getTransformUniform() const { return _transformUniform; }

		void addIBOLines(unsigned short* elements, int i);
		void addIBOTriangles(unsigned short* elements, int i);

		void expand(const int maxSprites);
		void flush(unsigned int texture, int size);

		int vboFloats() { return 4 * /* quad */ (2 + /* position(x,y)*/ 2 /* texture(x,y) */ + 4 /*color(r,g,b,a) */); }
		int iboFloats() { return _mode == SpriteBatchMode::TRIANGLES ? 6 : 8;}
		int vboVertices(int size) {	return  size * vboFloats();	}
		int iboVertices(int size) {	return size * iboFloats(); }
		int vboBytes(int size) { return vboVertices(size) * sizeof(float); }
		int iboBytes(int size) { return iboVertices(size) * sizeof(unsigned short); }
	
	};

}

#endif