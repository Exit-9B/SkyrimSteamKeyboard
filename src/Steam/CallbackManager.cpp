#include "CallbackManager.h"

namespace Steam
{
	CallbackManager* CallbackManager::GetSingleton()
	{
		static CallbackManager singleton{};
		return &singleton;
	}

	CallbackManager::CallbackManager()
		: _gamepadTextInputDismissedCallback{ this, &CallbackManager::OnGamepadTextInputDismissed }
	{
	}

	void CallbackManager::RegisterListener(
		ICallbackHandler<::GamepadTextInputDismissed_t>* a_handler)
	{
		_textInputHandler = a_handler;
	}

	void CallbackManager::OnGamepadTextInputDismissed(::GamepadTextInputDismissed_t* a_param)
	{
		if (_textInputHandler) {
			_textInputHandler->HandleEvent(a_param);
			_textInputHandler = nullptr;
		}
	}
}
