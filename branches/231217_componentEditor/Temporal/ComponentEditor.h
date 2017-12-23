#ifndef COMPONENT_EDITOR_H
#define COMPONENT_EDITOR_H

#include "EntitySystem.h"
#include "ToolComponent.h"
#include "Vector.h"
#include "Serialization.h"

namespace Temporal
{
	class ComponentEditor;

	class ComponentEditorSerializer
	{
	public:
		ComponentEditorSerializer(ComponentEditor& componentEditorSerializer) : _componentEditorSerializer(componentEditorSerializer) {}

		void serialize(const char* key, int& value) {};
		void serialize(const char* key, unsigned int& value) {};
		void serialize(const char* key, float& value);
		void serialize(const char* key, bool& value);
		void serialize(const char* key, Hash& value);
		void serialize(const char* key, Timer& value) {};
		void serializeRadians(const char* key, float& value) {};
		void serialize(const char* key, std::string& value) {};

		template<class T>
		void serialize(const char* key, T*& value)
		{
			if (!SerializationAccess::shouldSerialize(value))
				return;

			SerializationAccess::modifyKey(key, value);

			if (strcmp(key, "sensor") == 0 || strcmp(key, "animator") == 0 || strcmp(key, "renderer") == 0)
				return;

			preSerialize(key);
			SerializationAccess::serialize(key, value, *this);
			postSerialize(key);
		}

		template<class T>
		void serialize(const char* key, T& value)
		{
			preSerialize(key);
			SerializationAccess::serialize(key, value, *this);
			postSerialize(key);
		}

		template<class T>
		void serialize(const char* key, std::vector<T*>& value)
		{
			typedef std::vector<T*>::iterator TIterator;
			for (TIterator i = value.begin(); i != value.end(); ++i)
				serialize(key, *i);
		}

		template<class T>
		void serialize(const char* key, std::unordered_map<Hash, T*>& value)
		{
			std::map<Hash, T*> orderedValue(value.begin(), value.end());
			typedef std::map<Hash, T*>::iterator TIterator;
			for (TIterator i = orderedValue.begin(); i != orderedValue.end(); ++i)
				serialize(key, i->second);
		}

	private:
		void preSerialize(const char* key);
		void postSerialize(const char* key);

		ComponentEditor& _componentEditorSerializer;
		std::vector<std::string> _prefixes;

		void hashChanged(const char* controlValue, Hash& componentValue);
		void boolChanged(bool controlValue, bool& componentValue);
		void floatChanged(const char* controlValue, float& componentValue);

		ComponentEditorSerializer(const ComponentEditorSerializer&);
		ComponentEditorSerializer& operator=(const ComponentEditorSerializer&);
	};

	class ComponentEditor : public ToolComponent
	{
	public:
		ComponentEditor(Entity& entity) : ToolComponent(), _y(0.0f), _focused(false), _entity(entity), _componentEditorSerializer(*this) {}

		Hash getType() const { return Hash("component-editor"); }
		void handleMessage(Message& message);
		Component* clone() const { return 0; } 

		void addPanelTextBox(const char* label, const char* text, IAction1<const char*>* textChangedEvent);
		void addPanelCheckBox(const char* label, bool value, IAction1<bool>* checkChangedEvent);
	private:
		Vector PANEL_SIZE;
		Vector CONTROL_SIZE;

		HashList _ids;
		float _y;
		bool _focused;
		Entity& _entity;
		ComponentEditorSerializer _componentEditorSerializer;

		AABB getNextLabelShape();
		AABB getNextControlShape();
		void ok();
	};
}

#endif