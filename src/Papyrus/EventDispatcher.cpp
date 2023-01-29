#include "EventDispatcher.h"

#include "Steam/Steam.h"

namespace Papyrus
{
	EventDispatcher* EventDispatcher::GetSingleton()
	{
		static EventDispatcher singleton{};
		return &singleton;
	}

	void EventDispatcher::HandleEvent(::GamepadTextInputDismissed_t* a_param)
	{
		auto modEvent = SKSE::ModCallbackEvent{
			.eventName = "VirtualKeyboardClose",
			.strArg = Steam::GetTextInput(a_param),
			.numArg = static_cast<float>(!a_param->m_bSubmitted),
			.sender = nullptr,
		};

		auto eventSource = SKSE::GetModCallbackEventSource();
		eventSource->SendEvent(&modEvent);
	}
}
