#ifndef TEXT_H
#define TEXT_H
#include "EntitySystem.h"
#include <memory>
#include <ftgl/ftgl.h>

namespace Temporal
{
	class Text : public Component
	{
	public:
		
		explicit Text(std::shared_ptr<FTFont> font = 0, const char* text = 0) : _font(font), _text(text)  {}
		~Text() { delete[] _fontFamily; delete[] _text; }

		Hash getType() const { return TYPE; }

		void handleMessage(Message& message);
		Component* clone() const;

		static const Hash TYPE;
	private:

		FTSimpleLayout _layout;
		const char* _fontFamily;
		unsigned int _fontSize;
		std::shared_ptr<FTFont> _font;
		const char* _text;

		void draw();

		friend class SerializationAccess;
	};
}
#endif