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
	class TemporalPeriod;
	class Patrol;

	class ComponentEditor : public ToolComponent
	{
	public:
		ComponentEditor() : ToolComponent(20) {}

		void handleMessage(Message& message);

	protected:
		void addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent);
		void addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent);
	private:
		Vector PANEL_SIZE;
		Vector CONTROL_SIZE;

		static HashList _ids;
		static float _y;
		static bool _focused;

		AABB getNextLabelShape();
		AABB getNextControlShape();
		void ok();
	};

	class ButtonEditor : public ComponentEditor
	{
	public:
		ButtonEditor(Button& button) : _button(button) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Button& _button;

		void targetChanged(const char* s);
	};

	class LaserEditor : public ComponentEditor
	{
	public:
		LaserEditor(Laser& laser) : _laser(laser) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Laser& _laser;

		void friendlyChanged(bool b);
		void speedPerSecondChanged(const char* s);
	};

	class LightEditor : public ComponentEditor
	{
	public:
		LightEditor(Light& light) :_light(light) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Light& _light;

		void activatedChanged(bool b);
	};

	class DoorEditor : public ComponentEditor
	{
	public:
		DoorEditor(Door& door) : _door(door) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Door& _door;

		void closedChanged(bool b);
	};

	class TemporalPeriodEditor : public ComponentEditor
	{
	public:
		TemporalPeriodEditor(TemporalPeriod& period) : _period(period) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		TemporalPeriod& _period;

		void futureSelfIdChanged(const char* s);
		void createFutureSelfChanged(bool b);
		void syncFutureSelfChanged(bool b);
	};

	class PatrolEditor : public ComponentEditor
	{
	public:
		PatrolEditor(Patrol& patrol) : _patrol(patrol) {}

		Hash getType() const { return TYPE; }
		void handleMessage(Message& message);
		Component* clone() const { return 0; }

		static Hash TYPE;
	private:
		Patrol& _patrol;

		void isStaticChanged(bool b);
		void securityCameraIdChanged(const char* s);
	};
}

#endif