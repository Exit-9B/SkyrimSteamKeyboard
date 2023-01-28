#include "EventDispatcher.h"

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
			.strArg = GetTextInput(a_param),
			.numArg = static_cast<float>(!a_param->m_bSubmitted),
			.sender = nullptr,
		};

		auto eventSource = SKSE::GetModCallbackEventSource();
		eventSource->SendEvent(&modEvent);
	}

	RE::BSFixedString EventDispatcher::GetTextInput(::GamepadTextInputDismissed_t* a_param)
	{
		RE::BSFixedString textInput;

		if (!a_param->m_bSubmitted) {
			return textInput;
		}

		auto utils = ::SteamUtils();
		if (!utils) {
			return textInput;
		}

		uint32 length = utils->GetEnteredGamepadTextLength();
		if (length == 0) {
			return textInput;
		}

		auto buffer = std::make_unique<char[]>(length);
		utils->GetEnteredGamepadTextInput(buffer.get(), length);

		textInput = buffer.get();

		return textInput;
	}
}
