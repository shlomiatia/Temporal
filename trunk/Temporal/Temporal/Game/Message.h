#pragma once

#include "Enums.h"
#include <stdio.h>

namespace Temporal
{
	class Message
	{
	public:
		Message(MessageID::Enum id, void* param = NULL) : _id(id), _param(param) {}

		MessageID::Enum getID(void) const { return _id; }
		void* getParam(void) const { return _param; }
		void setParam(void* param) { _param = param; }

	private:
		MessageID::Enum _id;
		void* _param;

		Message(const Message&);
		Message& operator=(const Message&);
	};
}