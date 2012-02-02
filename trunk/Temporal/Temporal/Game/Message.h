#pragma once

#include <assert.h>
#include <Temporal\Base\Base.h>
#include "Enums.h"

namespace Temporal
{
	class Message
	{
	public:
		Message(MessageID::Type id, const void* const param = NULL) : _id(id), _param(param) {}

		MessageID::Type getID(void) const { return _id; }
		void setParam(const void* const param) { _param = param; }
		
		bool isNullParam(void) const { return getParam() == NULL; }

		template <class T>
		const T& getParam(void) const
		{
			assert(!isNullParam());
			return *((T*)getParam());
		}
		
	private:
		MessageID::Type _id;
		const void* _param;

		const void* const getParam(void) const { return _param; }

		Message(const Message&) {};
		Message& operator=(const Message&) {};
	};
}