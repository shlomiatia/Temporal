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

		MessageID::Enum getID() const { return _id; }
		void* getParam() const { return _param; }
		void setParam(void* param) { _param = param; }

	private:
		MessageID::Enum _id;
		void* _param;

		Message(const Message&);
		Message& operator=(const Message&);
	};
}
#endif