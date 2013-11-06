#ifndef CONTROL_H
#define CONTROL_H
#include <memory>
#include <string>
#include <ftgl/ftgl.h>
#include "EntitySystem.h"
#include "Delegate.h"
#include "Shapes.h"
#include "Color.h"
#include "Keyboard.h"

namespace Temporal
{
	class MouseParams;
	namespace Key { enum Enum; }

	class Control : public Component
	{
	public:
		explicit Control() : _box(YABP::Zero), _fontFamily("c:/windows/fonts/Arial.ttf"), _fontSize(12), _shortcutKey(Key::NONE),
			_backgroundColor(Color::Transparent), _foregroundColor(Color::White), _borderColor(Color::White), _hoverColor(Color::Transparent),
			_isTextBox(false), _isTextBoxMode(false), _isLeftDown(false), _isLeftClick(false), _isRightDown(false), _isRightClick(false),
			_leftMouseDownEvent(0), _leftMouseClickEvent(0), _leftMouseUpEvent(0), _rightMouseDownEvent(0), _rightMouseClickEvent(0), _rightMouseUpEvent(0), _mouseMoveEvent(0), _textChangedEvent(0), _commandEvent(0) {}
		~Control();

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return new Control(); }

		void setBackgroundColor(const Color& backgroundColor) { _backgroundColor = backgroundColor; }
		const Color& getBackgroundColor() const { return _backgroundColor; }
		void setForegroundColor(const Color& foregroundColor) { _foregroundColor = foregroundColor; }
		const Color& getForegroundColor() const { return _foregroundColor; }
		void setBorderColor(const Color& borderColor) { _borderColor = borderColor; }
		const Color& getBorderColor() const { return _borderColor; }
		void setWidth(float width);
		void Control::setHeight(float height) { _box.setHeight(height);	}
		float Control::getWidth() const	{ return _box.getWidth(); }
		float Control::getHeight() const { return _box.getHeight();	}

		void setText(const char* text) { getString() = text; }
		const char* getText() const { return getString().c_str(); }

		void setLeftMouseDownEvent(IAction1<const MouseParams&>* leftMouseDownEvent) { setEvent(_leftMouseDownEvent, leftMouseDownEvent); }
		void setLeftMouseClickEvent(IAction1<const MouseParams&>* leftMouseClickEvent) { setEvent(_leftMouseClickEvent, leftMouseClickEvent); }
		void setLeftMouseUpEvent(IAction1<const MouseParams&>* leftMouseUpEvent) { setEvent(_leftMouseUpEvent, leftMouseUpEvent); }
		void setRightMouseDownEvent(IAction1<const MouseParams&>* rightMouseDownEvent) { setEvent(_rightMouseDownEvent, rightMouseDownEvent); }
		void setRightMouseClickEvent(IAction1<const MouseParams&>* rightMouseClickEvent) { setEvent(_rightMouseClickEvent, rightMouseClickEvent); }
		void setRightMouseUpEvent(IAction1<const MouseParams&>* rightMouseUpEvent) { setEvent(_rightMouseUpEvent, rightMouseUpEvent); }
		void setMouseMoveEvent(IAction1<const MouseParams&>* mouseMoveEvent) { setEvent(_mouseMoveEvent, mouseMoveEvent); }
		void setTextChangedEvent(IAction1<const char*>* textChangedEvent);
		void setCommandEvent(IAction* commandEvent);

		void setShortcutKey(Key::Enum key) { _shortcutKey = key; }

		static const Hash TYPE;
	private:
		std::string _fontFamily;
		unsigned int _fontSize;
		FTSimpleLayout _layout;
		std::shared_ptr<FTFont> _font;

		YABP _box;

		Color _backgroundColor;
		Color _foregroundColor;
		Color _borderColor;
		Color _hoverColor;

		bool _isTextBox;
		bool _isTextBoxMode;
		std::string _label;
		std::string _textbox;

		IAction1<const MouseParams&>* _leftMouseDownEvent;
		IAction1<const MouseParams&>* _leftMouseClickEvent;
		IAction1<const MouseParams&>* _leftMouseUpEvent;
		IAction1<const MouseParams&>* _rightMouseDownEvent;
		IAction1<const MouseParams&>* _rightMouseClickEvent;
		IAction1<const MouseParams&>* _rightMouseUpEvent;
		IAction1<const MouseParams&>* _mouseMoveEvent;
		IAction1<const char*>* _textChangedEvent;
		IAction* _commandEvent;

		bool _isLeftDown;
		bool _isLeftClick;
		bool _isRightDown;
		bool _isRightClick;

		Key::Enum _shortcutKey;

		void draw();
		void setEvent(IAction1<const MouseParams&>*& prop, IAction1<const MouseParams&>* value);
		const std::string& getString() const { return _isTextBoxMode ? _textbox : _label; }
		std::string& getString() { return _isTextBoxMode ? _textbox : _label; }

		friend class SerializationAccess;
	};
}
#endif