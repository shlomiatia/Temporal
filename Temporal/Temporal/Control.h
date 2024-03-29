#ifndef CONTROL_H
#define CONTROL_H
#include <memory>
#include <string>
#include "EntitySystem.h"
#include "Delegate.h"
#include "Shapes.h"
#include "Color.h"
#include "InputEnums.h"
#include "Ids.h"

namespace Temporal
{
	class MouseParams;
	class Font;
	namespace Key { enum Enum; }

	class Control : public Component
	{
	public:
		explicit Control() : _obb(OBB::Zero), _fontFamily("c:/windows/fonts/Arial.ttf"), _fontSize(16), _shortcutKey(Key::NONE),
			_backgroundColor(0.2890625f, 0.73828125f, 0.671875f, 0.9f), _foregroundColor(0.87109375f, 0.859375f, 0.88671875f), _borderColor(0.21484375f, 0.21484375f, 0.21484375f), _hoverColor(0.96484375f, 0.71484375f, 0.19921875f, 0.9f),
			_isTextBox(false), _isTextBoxMode(false), _isVisible(true), _isLeftDown(false), _isLeftClick(false), _isMiddleDown(false), _isMiddleClick(false), _isRightDown(false), _isRightClick(false), _isHover(false), _isCheckbox(false),
			_leftMouseDownEvent(0), _leftMouseClickEvent(0), _leftMouseUpEvent(0), _middleMouseDownEvent(0), _middleMouseClickEvent(0), _middleMouseUpEvent(0), _rightMouseDownEvent(0), _rightMouseClickEvent(0), _rightMouseUpEvent(0),
			_mouseMoveEvent(0), _textChangedEvent(0), _commandEvent(0), _checkChangedEvent(0),
			_box(&_obb) {}
		~Control();

		Hash getType() const { return ComponentsIds::CONTROL; }
		void handleMessage(Message& message);
		Component* clone() const { return new Control(); }

		void setFontSize(int fontSize) { _fontSize = fontSize; }
		int getFontSize() const { return _fontSize; }
		void setBackgroundColor(const Color& backgroundColor) { _backgroundColor = backgroundColor; }
		const Color& getBackgroundColor() const { return _backgroundColor; }
		void setForegroundColor(const Color& foregroundColor) { _foregroundColor = foregroundColor; }
		const Color& getForegroundColor() const { return _foregroundColor; }
		void setBorderColor(const Color& borderColor) { _borderColor = borderColor; }
		const Color& getBorderColor() const { return _borderColor; }
		void setHoverColor(const Color& color) { _hoverColor = color; }
		const Color& getHoverColor() const { return _hoverColor; }
		void setWidth(float width) { _box.setWidth(width);	}
		void setHeight(float height) { _box.setHeight(height);	}
		float getWidth() const	{ return _box.getWidth(); }
		float getHeight() const { return _box.getHeight();	}
		void setText(const char* text) { if(text) _label = text; }
		const char* getText() const { return _label.c_str(); }
		void setVisible(bool isVisible) { _isVisible = isVisible; }
		bool isVisible() const { return _isVisible; }

		void setLeftMouseDownEvent(IAction1<const MouseParams&>* leftMouseDownEvent) { setEvent(_leftMouseDownEvent, leftMouseDownEvent); }
		void setLeftMouseClickEvent(IAction1<const MouseParams&>* leftMouseClickEvent) { setEvent(_leftMouseClickEvent, leftMouseClickEvent); }
		void setLeftMouseUpEvent(IAction1<const MouseParams&>* leftMouseUpEvent) { setEvent(_leftMouseUpEvent, leftMouseUpEvent); }
		void setMiddleMouseDownEvent(IAction1<const MouseParams&>* middleMouseDownEvent) { setEvent(_middleMouseDownEvent, middleMouseDownEvent); }
		void setMiddleMouseClickEvent(IAction1<const MouseParams&>* middleMouseClickEvent) { setEvent(_middleMouseClickEvent, middleMouseClickEvent); }
		void setMiddleMouseUpEvent(IAction1<const MouseParams&>* middleMouseUpEvent) { setEvent(_middleMouseUpEvent, middleMouseUpEvent); }
		void setRightMouseDownEvent(IAction1<const MouseParams&>* rightMouseDownEvent) { setEvent(_rightMouseDownEvent, rightMouseDownEvent); }
		void setRightMouseClickEvent(IAction1<const MouseParams&>* rightMouseClickEvent) { setEvent(_rightMouseClickEvent, rightMouseClickEvent); }
		void setRightMouseUpEvent(IAction1<const MouseParams&>* rightMouseUpEvent) { setEvent(_rightMouseUpEvent, rightMouseUpEvent); }
		void setMouseMoveEvent(IAction1<const MouseParams&>* mouseMoveEvent) { setEvent(_mouseMoveEvent, mouseMoveEvent); }
		void setTextChangedEvent(IAction1<const char*>* textChangedEvent);
		void setCheckChangedEvent(IAction1<bool>* checkChangedEvent);
		void setCommandEvent(IAction* commandEvent);

		void setShortcutKey(Key::Enum key) { _shortcutKey = key; }
		void focus();
		void blur();
		
	private:
		std::string _fontFamily;
		unsigned int _fontSize;
		std::shared_ptr<Font> _font;

		OBB _obb;
		OBBAABBWrapper _box;

		Color _backgroundColor;
		Color _foregroundColor;
		Color _borderColor;
		Color _hoverColor;

		bool _isTextBox;
		bool _isTextBoxMode;
		bool _isCheckbox;
		bool _isVisible;
		std::string _label;

		IAction1<const MouseParams&>* _leftMouseDownEvent;
		IAction1<const MouseParams&>* _leftMouseClickEvent;
		IAction1<const MouseParams&>* _leftMouseUpEvent;
		IAction1<const MouseParams&>* _middleMouseDownEvent;
		IAction1<const MouseParams&>* _middleMouseClickEvent;
		IAction1<const MouseParams&>* _middleMouseUpEvent;
		IAction1<const MouseParams&>* _rightMouseDownEvent;
		IAction1<const MouseParams&>* _rightMouseClickEvent;
		IAction1<const MouseParams&>* _rightMouseUpEvent;
		IAction1<const MouseParams&>* _mouseMoveEvent;
		IAction1<const char*>* _textChangedEvent;
		IAction1<bool>* _checkChangedEvent;
		IAction* _commandEvent;

		bool _isLeftDown;
		bool _isLeftClick;
		bool _isMiddleDown;
		bool _isMiddleClick;
		bool _isRightDown;
		bool _isRightClick;
		bool _isHover;

		Key::Enum _shortcutKey;

		void drawControl();
		void drawText();
		void drawBorder();
		void setEvent(IAction1<const MouseParams&>*& prop, IAction1<const MouseParams&>* value);
		void buttonKeyUp(Key::Enum key);
		void textBoxKeyUp(Key::Enum key);
		void mouseDown(MouseParams& params);
		void mouseUp(MouseParams& params);
		void mouseMove(MouseParams& params);

		friend class SerializationAccess;
	};
}
#endif