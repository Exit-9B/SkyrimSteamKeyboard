#pragma once

#include "Steam/CallbackManager.h"

namespace Papyrus
{
	class EventDispatcher final : public Steam::ICallbackHandler<::GamepadTextInputDismissed_t>
	{
	public:
		static EventDispatcher* GetSingleton();

		EventDispatcher(const EventDispatcher&) = delete;
		EventDispatcher(EventDispatcher&&) = delete;
		EventDispatcher& operator=(const EventDispatcher&) = delete;
		EventDispatcher& operator=(EventDispatcher&&) = delete;

		void HandleEvent(::GamepadTextInputDismissed_t* a_param) override;

	private:
		EventDispatcher() = default;
	};
}
