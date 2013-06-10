#ifndef TEXT_H
#define TEXT_H
#include "EntitySystem.h"
#include <ftgl/ftgl.h>

namespace Temporal
{
	class Text : public Component
	{
	public:
		
		explicit Text(FTFont* font = 0, char* text = 0) : _font(font), _text(text)  {}
		~Text() { delete[] _fontFamily; delete[] _text; }

		ComponentType::Enum getType() const { return ComponentType::RENDERER; }

		void handleMessage(Message& message);
		Component* clone() const;

	private:
		FTSimpleLayout _layout;
		char* _fontFamily;
		unsigned int _fontSize;
		FTFont* _font;
		char* _text;

		void draw();

		friend class SerializationAccess;
	};
}
#endif