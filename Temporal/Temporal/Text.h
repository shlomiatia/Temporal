#ifndef TEXT_H
#define TEXT_H
#include "EntitySystem.h"
#include <memory>
#include <string>
#include <ftgl/ftgl.h>

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

		static const Hash TYPE;
	private:

		FTSimpleLayout _layout;
		std::string _fontFamily;
		unsigned int _fontSize;
		std::shared_ptr<FTFont> _font;
		std::string _text;

		void draw();

		friend class SerializationAccess;
	};
}
#endif