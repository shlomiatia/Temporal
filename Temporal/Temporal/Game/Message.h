#ifndef MESSAGE_H
#define MESSAGE_H

#include "GameEnums.h"
#include <stdio.h>

namespace Temporal
{
	class Message
	{
	public:
		explicit Message(MessageID::Enum id, void* param = NULL) : _id(id), _param(param) {}

		MessageID::Enum getID(void) const { return _id; }
		void* getParam(void) const { return _param; }
		void setParam(void* param) { _param = param; }
		ComponentType::Enum getSender(void) const { return _sender; }
		void setSender(ComponentType::Enum sender) { _sender = sender; }

	private:
		MessageID::Enum _id;
		void* _param;
		ComponentType::Enum _sender;

		Message(const Message&);
		Message& operator=(const Message&);
	};
}
#endif