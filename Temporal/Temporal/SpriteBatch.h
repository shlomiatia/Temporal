#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "Vector.h"
#include "Shapes.h"
#include "Color.h"
#include <vector>
#include <glm/glm.hpp>

namespace Temporal
{
	class Texture;
	class ShaderProgram;

	class SpriteBatchItem
	{
	public:
		SpriteBatchItem() : texture(0), color(Color::White) {}

		const Texture* texture;
		Vector a;
		Vector b;
		Vector c;
		Vector d;
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

		SpriteBatch(ShaderProgram& program, SpriteBatchMode::Enum mode = SpriteBatchMode::TRIANGLES) : 
			_program(program), _mode(mode), _size(0), _vertices(0), _vao(0), _vbo(0), _ibo(0), _lastTexture(0), _coordinateAttribute(0), _textureCoordinateAttribute(0), _colorAttribute(0),
			_textureUniform(0), _typeUniform(0) {}
		~SpriteBatch();

		int getTypeUniform() const { return _typeUniform; }

		void init();
		void begin();

		// obb
		void add(const Texture* texture, const Vector& translation, const AABB& texturePart = AABB(0.5, 0.5f, 1.0f, 1.0f), const Color& color = Color::White, float rotation = 0.0f, 
			const Vector& pivot = Vector::Zero, const Vector& scale = Vector(1.0f, 1.0f), bool flipX = false, bool flipY = false, const Vector& radius = Vector::Zero, bool ignoreMatrixStack = false);

		// quad
		void add(const Texture* texture, const Vector& alpha, const Vector& beta, const Vector& gamma, const Vector& delta, const AABB& texturePart = AABB::Zero, const Color& color = Color::White, bool ignoreMatrixStack = false);

		// OBB
		void add(const OBB& obb, const Color& color = Color::White)
		{
			add(0, obb.getCenter(), AABB::Zero, color, obb.getAngle(), Vector::Zero, Vector::Zero, false, false, obb.getRadius());
		}
		// AABB
		void add(const AABB& aabb, const Color& color = Color::White)
		{
			add(0, aabb.getCenter(), AABB::Zero, color, 0.0f, Vector::Zero, Vector::Zero, false, false, aabb.getRadius());
		}
		void end();
	
	private:
		static const int INITLAL_MAX_SPRITES = 256;

		ShaderProgram& _program;

		SpriteBatchMode::Enum _mode;
		
		std::vector<SpriteBatchItem> _items;
		int _size;
		float* _vertices;
		unsigned int _vao, _vbo, _ibo;
		
		unsigned int _lastTexture;

		int _coordinateAttribute, _textureCoordinateAttribute, _colorAttribute;
		int _textureUniform, _typeUniform;

		int getCoordinateAttribute() const { return _coordinateAttribute; }
		int getTextureCoordinateAttribute() const { return _textureCoordinateAttribute; }
		int getColorAttribute() const { return _colorAttribute; }
		int getTextureUniform() const { return _textureUniform; }
		

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

		void innerAdd(const Texture* texture, const Vector& a, const Vector& b, const Vector& c, const Vector& d, const AABB& actualTexturePart, const Color& color);
	};

}

#endif