#ifndef COMPONENT_EDITORS_H
#define COMPONENT_EDITORS_H

#include "EntitySystem.h"
#include "ToolComponent.h"
#include "Vector.h"

namespace Temporal
{
	class MovingPlatform;
	class Button;
	class Laser;
	class Light;
	class Door;

	class ComponentEditor : public ToolComponent
	{
	public:
		ComponentEditor() : ToolComponent(20), _y(0.0f) {}

		void handleMessage(Message& message);

	protected:
		void addPanelTextBox(Hash id, const char* text, IAction1<const char*>* textChangedEvent);
		void addPanelCheckBox(Hash id, bool value, IAction1<bool>* checkChangedEvent);
	private:
		Vector PANEL_SIZE;
		Vector CONTROL_SIZE;

		float _y;
		HashList _ids;

		AABB getNextControlShape();
		void ok();
	};

	class MovingPlatformEditor : public ComponentEditor
	{
	public:
		MovingPlatformEditor(MovingPlatform& movingPlatform) : _movingPlatform(movingPlatform) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		MovingPlatform& _movingPlatform;

		void movementXChanged(const char* s);
		void movementYChanged(const char* s);
	};

	class ButtonEditor : public ComponentEditor
	{
	public:
		ButtonEditor(Button& button) : _button(button) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		Button& _button;

		void targetChanged(const char* s);
	};

	class LaserEditor : public ComponentEditor
	{
	public:
		LaserEditor(Laser& laser) : _laser(laser) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		Laser& _laser;

		void friendlyChanged(bool b);
	};

	class LightEditor : public ComponentEditor
	{
	public:
		LightEditor(Light& light) :_light(light) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		Light& _light;

		void activatedChanged(bool b);
	};

	class DoorEditor : public ComponentEditor
	{
	public:
		DoorEditor(Door& door) : _door(door) {}

		Hash getType() const { return Hash::INVALID; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }
	private:
		Door& _door;

		void closedChanged(bool b);
	};
}

#endif