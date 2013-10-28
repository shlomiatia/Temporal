#ifndef TEXT_H
#define TEXT_H
#include "EntitySystem.h"
#include <memory>
#include <string>
#include <ftgl/ftgl.h>

#include "Shapes.h"
#include "Mouse.h"

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

	class Panel : public MouseListener
	{
	public:
		explicit Panel(const Vector& radius = Vector::Zero) : _shape(YABPAABB(Vector::Zero, radius)), _fill(false) {}
		explicit Panel(const YABP& shape = YABP::Zero) : _shape(shape), _fill(false) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Panel(_shape); }

		void setFill(bool fill) { _fill = fill; }
		bool isFill() const { return _fill; }

		static const Hash TYPE;
	private:
		YABP _shape;
		bool _fill;
	};
}
#endif