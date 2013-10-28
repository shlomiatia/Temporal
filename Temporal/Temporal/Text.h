#ifndef TEXT_H
#define TEXT_H
#include "EntitySystem.h"
#include <memory>
#include <string>
#include <ftgl/ftgl.h>

#include "Shapes.h"
#include "Mouse.h"
#include "Color.h"

namespace Temporal
{
	class Text : public Component
	{
	public:
		
		explicit Text(const char* fontFamily = "", unsigned int size = 0, const char* text = "") : _fontFamily(fontFamily), _fontSize(size), _text(text)  {}

		Hash getType() const { return TYPE; }

		void setText(const char* text) { _text = text; }
		void setWidth(float width);
		float getWidth() const;
		float getHeight() const;

		void handleMessage(Message& message);
		Component* clone() const;
		
	private:
		static const Hash TYPE;

		FTSimpleLayout _layout;
		std::string _fontFamily;
		unsigned int _fontSize;
		std::shared_ptr<FTFont> _font;
		std::string _text;

		void draw();

		friend class SerializationAccess;
	};

	class Control : public MouseListener
	{
	public:
		explicit Control(const Vector& radius = Vector::Zero) : _shape(YABPAABB(Vector::Zero, radius)), _color(Color::White), _fill(false) {}
		explicit Control(const YABP& shape = YABP::Zero) : _shape(shape), _color(Color::White), _fill(false) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Control(_shape); }

		void setFill(bool fill) { _fill = fill; }
		bool isFill() const { return _fill; }
		void setColor(const Color& color) { _color = color; }
		const Color& getColor() const { return _color; }

		static const Hash TYPE;
	private:
		YABP _shape;
		Color _color;
		bool _fill;
	};
}
#endif