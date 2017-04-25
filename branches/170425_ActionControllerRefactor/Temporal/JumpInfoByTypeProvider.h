#pragma once

#include "JumpInfo.h"
#include "ScriptsEnums.h"

namespace Temporal
{
	class JumpInfoByTypeProvider
	{
	public:
		explicit JumpInfoByTypeProvider(JumpType::Enum type) : _type(type) {}

		JumpType::Enum getType() const { return _type; }
		const JumpInfo& getInfo() const { return _type == JumpType::UP ? JUMP_UP_INFO : JUMP_FORWARD_INFO; }
	private:
		static const JumpInfo JUMP_UP_INFO;
		static const JumpInfo JUMP_FORWARD_INFO;

		JumpType::Enum _type;

		JumpInfoByTypeProvider(const JumpInfoByTypeProvider&);
		JumpInfoByTypeProvider& operator=(const JumpInfoByTypeProvider&);

		friend class SerializationAccess;
	};
}