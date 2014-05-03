#include "SpriteBatch.h"
#include "Graphics.h"
#include "Texture.h"
#include "Math.h"
#include "ShaderProgram.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

		_program.setUniform(_typeUniform, 0);

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

	void SpriteBatch::add(const Texture* texture, const Vector& translation, const AABB& texturePart, const Color& color, float rotation, const Vector& pivot, const Vector& scale, bool flipX, bool flipY, const Vector& radius)
	{
		glm::mat4 m = Graphics::get().getMatrixStack().top();
		m = glm::translate(m, glm::vec3(translation.getX(), translation.getY(), 0.0f));
		m = glm::rotate(m, fromRadians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		m = glm::translate(m, glm::vec3(pivot.getX(), pivot.getY(), 0.0f));
		Vector actualScale = scale;
		if(flipX)
			actualScale.setX(-actualScale.getX());
		if(flipY)
			actualScale.setX(-actualScale.getY());
		m = glm::scale(m, glm::vec3(actualScale.getX(), actualScale.getY(), 0.0f));
		
		add(texture, texturePart, color, radius, m);
	}

	void SpriteBatch::add(const Texture* texture, const AABB& texturePart, const Color& color, const Vector& radius, const glm::mat4& m)
	{
		if(_size == _items.size())
		{
			int size = _items.size() == 0 ? INITLAL_MAX_SPRITES : _items.size() * 2;
			_items.resize(size);
			expand(_items.size());
		}
		
		Vector actualRadius = Vector::Zero; radius != Vector::Zero ? radius : texturePart.getRadius();
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
		SpriteBatchItem& item = _items[_size];
		item.texture = texture;
		if(texture) {
			if(texturePart == AABB::Zero)
				item.texturePart = AABB(0.5f, 0.5f, 1.0f, 1.0f);
			else
				item.texturePart = AABB(Vector(texturePart.getCenterX() / texture->getSize().getX(), texturePart.getCenterY() / texture->getSize().getY()),
										Vector(texturePart.getRadiusX() / texture->getSize().getX(), texturePart.getRadiusY() / texture->getSize().getY()));
		}
			
		glm::vec4 a = m * glm::vec4(-actualRadius.getX(), -actualRadius.getY(), 0.0f, 1.0f);
		glm::vec4 b = m * glm::vec4(actualRadius.getX(), -actualRadius.getY(), 0.0f, 1.0f);
		glm::vec4 c = m * glm::vec4(actualRadius.getX(), actualRadius.getY(), 0.0f, 1.0f);
		glm::vec4 d = m * glm::vec4(-actualRadius.getX(), actualRadius.getY(), 0.0f, 1.0f);
		item.a = Vector(a.x, a.y);
		item.b = Vector(b.x, b.y);
		item.c = Vector(c.x, c.y);
		item.d = Vector(d.x, d.y);
		item.color = color;
		++_size;
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