#pragma once

#include "Enums.h"
#include <assert.h>
#include <stdio.h>

namespace Temporal
{
	class Message
	{
	public:
		Message(MessageID::Enum id, const void* const param = NULL) : _id(id), _param(param) {}

		MessageID::Enum getID(void) const { return _id; }
		const void* const getParam(void) const { return _param; }
		void setParam(const void* const param) { _param = param; }

	private:
		MessageID::Enum _id;
		const void* _param;

		Message(const Message&);
		Message& operator=(const Message&);
	};
}