#include "SpriteBatch.h"
#include "Graphics.h"
#include "Texture.h"
#include "Math.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include <GL/glew.h>
#include <algorithm>

namespace Temporal
{
	inline bool compare(const SpriteBatchItem& a, const SpriteBatchItem& b)
	{
		return a.getTextureId() < b.getTextureId();
	}

	unsigned int SpriteBatchItem::getTextureId() const 
	{
		return texture == 0 ? 0 : texture->getID(); 
	}

	void SpriteBatch::init() 
	{
		_coordinateAttribute = _program.getAttribute("a_coordinate");
		_textureCoordinateAttribute = _program.getAttribute("a_textureCoordinate");
		_colorAttribute = _program.getAttribute("a_color");
	
		_textureUniform = _program.getUniform("u_texture");
	
		_typeUniform = _program.getUniform("u_type");

		glGenBuffers(1, &_vbo);
		glGenBuffers(1, &_ibo);

		expand(INITLAL_MAX_SPRITES);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glEnableVertexAttribArray(getCoordinateAttribute());
	
		glVertexAttribPointer(
			getCoordinateAttribute(), // attribute
			2,                 // number of elements per vertex, here (x,y)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			vboFloats(),                 // no extra data between each position
			0                  // offset of first element
		);

		glEnableVertexAttribArray(getTextureCoordinateAttribute());
		glVertexAttribPointer(
			getTextureCoordinateAttribute(), // attribute
			2,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			vboFloats(),                 // no extra data between each position
			(void*)8			// offset of first element
		);

		glEnableVertexAttribArray(getColorAttribute());
		glVertexAttribPointer(
			getColorAttribute(), // attribute
			4,                  // number of elements per vertex, here (x,y)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			vboFloats(),                 // no extra data between each position
			(void*)16    // offset of first element
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

		glBindVertexArray(0);
		glDisableVertexAttribArray(getCoordinateAttribute());
		glDisableVertexAttribArray(getTextureCoordinateAttribute());
		glDisableVertexAttribArray(getColorAttribute());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	SpriteBatch::~SpriteBatch()
	{
		delete[] _vertices;
		glDeleteBuffers(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
	}

	void SpriteBatch::addIBOLines(unsigned short* elements, int i)
	{
		int k = i * 8;
		int j = i * 4;
		elements[k++] = j;
		elements[k++] = j+1;
		elements[k++] = j+1;
		elements[k++] = j+2;
		elements[k++] = j+2;
		elements[k++] = j+3;
		elements[k++] = j+3;
		elements[k++] = j;
	}

	void SpriteBatch::addIBOTriangles(unsigned short* elements, int i)
	{
		int k = i * 6;
		int j = i * 4;
		elements[k++] = j;
		elements[k++] = j+1;
		elements[k++] = j+2;
		elements[k++] = j+2;
		elements[k++] = j+3;
		elements[k++] = j;
	}

	void SpriteBatch::expand(const int maxSprites)
	{
		if(_vertices)
			delete[] _vertices;
		_vertices = new float[vboVertices(maxSprites)];
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, vboBytes(maxSprites), _vertices, GL_DYNAMIC_DRAW);
		
		GLushort* elements= new GLushort[iboVertices(maxSprites)];
		for(int i = 0; i < maxSprites; ++i)
		{
			_mode == SpriteBatchMode::TRIANGLES ? addIBOTriangles(elements, i) : addIBOLines(elements, i);
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iboBytes(maxSprites), elements, GL_STATIC_DRAW);
		delete elements;
	}

	void SpriteBatch::begin()
	{
		_lastTexture = 0;
		_size = 0;
	}

	AABB getActualTexturePart(const Texture* texture, const AABB& texturePart)
	{
		AABB actualTexturePart = AABB::Zero;
		if(texturePart == AABB::Zero)
			actualTexturePart = AABB(0.5f, 0.5f, 1.0f, 1.0f);
		else if(texture)
			actualTexturePart = AABB(Vector(texturePart.getCenterX() / texture->getSize().getX(), texturePart.getCenterY() / texture->getSize().getY()),
										Vector(texturePart.getRadiusX() / texture->getSize().getX(), texturePart.getRadiusY() / texture->getSize().getY()));
		return actualTexturePart;
	}

	Vector getActualScale(const Vector& scale, bool flipX, bool flipY)
	{
		Vector actualScale = scale;
		if(actualScale == Vector::Zero)
			actualScale = Vector(1.0f, 1.0f);
		if(flipX)
			actualScale.setX(-actualScale.getX());
		if(flipY)
			actualScale.setX(-actualScale.getY());
		return actualScale;
	}

	Vector getActualRadius(const Vector& radius, const Texture* texture, const AABB& texturePart)
	{
		Vector actualRadius = Vector::Zero;
		if(radius != Vector::Zero)
			actualRadius = radius;
		else if(texturePart != AABB::Zero)
			actualRadius = texturePart.getRadius();
		else
			actualRadius = texture->getSize() / 2.0f;
		if(actualRadius.getY() == 0.0f)
			actualRadius.setY(1.0f);
		else if(actualRadius.getX() == 0.0f)
			actualRadius.setX(1.0f);
		return actualRadius;
	}

	void SpriteBatch::innerAdd(const Texture* texture, const Vector& a, const Vector& b, const Vector& c, const Vector& d, const AABB& actualTexturePart, const Color& color)
	{
		// Expanding if needed
		if(_size == _items.size())
		{
			int size = _items.size() == 0 ? INITLAL_MAX_SPRITES : _items.size() * 2;
			_items.resize(size);
			expand(_items.size());
		}

		// Add item
		SpriteBatchItem& item = _items[_size];
		item.texture = texture;
		item.texturePart = actualTexturePart;
		item.a = a;
		item.b = b;
		item.c = c;
		item.d = d;
		item.color = color;
		++_size;
	}

	void SpriteBatch::add(const Texture* texture, const Vector& translation, const AABB& texturePart, const Color& color, float rotation, const Vector& pivot, const Vector& scale, bool flipX, bool flipY, 
		const Vector& radius, bool ignoreMatrixStack)
	{
		// Setting default value
		AABB actualTexturePart = getActualTexturePart(texture, texturePart);
		Vector actualScale = getActualScale(scale, flipX, flipY);
		Vector actualRadius = getActualRadius(radius, texture, texturePart);

		// Calculate matrix
		Matrix m = ignoreMatrixStack ? Matrix() : Graphics::get().getMatrixStack().top();
		m.translate(translation);
		m.rotate(AngleUtils::radiansToDegrees(rotation));
		m.translate(pivot);
		m.scale(actualScale);

		Vector a = m * Vector(-actualRadius.getX(), -actualRadius.getY());
		Vector b = m * Vector(actualRadius.getX(), -actualRadius.getY());
		Vector c = m * Vector(actualRadius.getX(), actualRadius.getY());
		Vector d = m * Vector(-actualRadius.getX(), actualRadius.getY());

		innerAdd(texture, a, b, c, d, actualTexturePart, color);
	}

	void SpriteBatch::add(const Texture* texture, const Vector& alpha, const Vector& beta, const Vector& gamma, const Vector& delta, const AABB& texturePart, const Color& color, bool ignoreMatrixStack)
	{
		AABB actualTexturePart = getActualTexturePart(texture, texturePart);
		Matrix m = ignoreMatrixStack ? Matrix() : Graphics::get().getMatrixStack().top();
		Vector a = m * alpha;
		Vector b = m * beta;
		Vector c = m * gamma;
		Vector d = m * delta;

		innerAdd(texture, a, b, c, d, actualTexturePart, color);
	}

	void SpriteBatch::end()
	{
		std::sort(_items.begin(), _items.begin() + _size, compare);

		int index = 0;
		for(SpriteBatchItemIterator itemI = _items.begin(); itemI != _items.begin() + _size; ++itemI)
		{	
			const SpriteBatchItem& item = *itemI;
			unsigned int texture = item.getTextureId();
			if(_lastTexture != texture)
			{
				flush(_lastTexture, index);
				_lastTexture = texture;
				index = 0;
			}
			
			int i = index * vboFloats();
			++index;

			_vertices[i++] = item.a.getX();
			_vertices[i++] = item.a.getY();
			_vertices[i++] = item.texturePart.getLeft();
			_vertices[i++] = item.texturePart.getTop();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = item.b.getX();
			_vertices[i++] = item.b.getY();
			_vertices[i++] = item.texturePart.getRight();
			_vertices[i++] = item.texturePart.getTop();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = item.c.getX();
			_vertices[i++] = item.c.getY();
			_vertices[i++] = item.texturePart.getRight();
			_vertices[i++] = item.texturePart.getBottom();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
			_vertices[i++] = item.d.getX(); 
			_vertices[i++] = item.d.getY();
			_vertices[i++] = item.texturePart.getLeft();
			_vertices[i++] = item.texturePart.getBottom();
			_vertices[i++] = item.color.getR();
			_vertices[i++] = item.color.getG();
			_vertices[i++] = item.color.getB();
			_vertices[i++] = item.color.getA();
		}
		flush(_lastTexture, index);
	}

	void SpriteBatch::flush(GLuint texture, int size)
	{
		if(size == 0)
			return;
		glUseProgram(_program.getId());
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(_vao);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(getTextureUniform(), 0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		
		int bytes = vboBytes(size);
		glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, _vertices);
		glDrawElements(_mode == SpriteBatchMode::TRIANGLES ? GL_TRIANGLES : GL_LINES, iboVertices(size), GL_UNSIGNED_SHORT, 0);

		glBindVertexArray(0);	
		glUseProgram(0);
		Graphics::get().validate();
	}
}