#pragma once

#include <assert.h>
#include <Temporal\Base\Base.h>
#include "Enums.h"

namespace Temporal
{
	class Message
	{
	public:
		Message(MessageID::Enum id, const void* const param = NULL) : _id(id), _param(param) {}

		MessageID::Enum getID(void) const { return _id; }
		void setParam(const void* const param) { _param = param; }

		// TODO: Force external users to const cast SLOTH!
		const void* const getParam(void) const { return _param; }

	private:
		MessageID::Enum _id;
		const void* _param;

		Message(const Message&);
		Message& operator=(const Message&);
	};
}