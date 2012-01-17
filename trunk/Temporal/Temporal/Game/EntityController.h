#pragma once

namespace Temporal
{
	class EntityController
	{
	public:
		EntityController(void) {}
		virtual ~EntityController(void) {}

		virtual bool isLeft(void) const = 0;
		virtual bool isRight(void) const = 0;
		virtual bool isUp(void) const = 0;
		virtual bool isDown(void) const = 0;
	private:
		EntityController(const EntityController&) {};
		EntityController& operator=(const EntityController&) {};
	};
}

