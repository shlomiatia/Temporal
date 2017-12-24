#include "ComponentEditorSerializer.h"
#include "Utils.h"
#include "Delegate.h"
#include "Hash.h"
#include "ComponentEditor.h"

namespace Temporal
{
	void ComponentEditorSerializer::serialize(const char* key, float& value)
	{
		std::string label = Utils::format("%s%s", Utils::join(_prefixes, " ").c_str(), key);
		_componentEditor.addPanelTextBox(label.c_str(), Utils::toString(value).c_str(), createClosureAction1(ComponentEditorSerializer, const char*, floatChanged, float&, value));
	}

	void ComponentEditorSerializer::serialize(const char* key, bool& value)
	{
		std::string label = Utils::format("%s%s", Utils::join(_prefixes, " ").c_str(), key);
		_componentEditor.addPanelCheckBox(label.c_str(), value, createClosureAction1(ComponentEditorSerializer, bool, boolChanged, bool&, value));
	}

	void ComponentEditorSerializer::serialize(const char* key, Hash& value)
	{
		std::string label = Utils::format("%s%s", Utils::join(_prefixes, " ").c_str(), key);
		_componentEditor.addPanelTextBox(label.c_str(), value.getString(), createClosureAction1(ComponentEditorSerializer, const char*, hashChanged, Hash&, value));
	}

	void ComponentEditorSerializer::hashChanged(const char* controlValue, Hash& componentValue)
	{
		componentValue = Hash(controlValue);
	}

	void ComponentEditorSerializer::boolChanged(bool controlValue, bool& componentValue)
	{
		componentValue = controlValue;
	}

	void ComponentEditorSerializer::floatChanged(const char* controlValue, float& componentValue)
	{
		componentValue = Utils::parseFloat(controlValue);
	}

	void ComponentEditorSerializer::preSerialize(const char* key)
	{
		_prefixes.push_back(key);
	}

	void ComponentEditorSerializer::postSerialize(const char* key)
	{
		_prefixes.pop_back();
	}
}
