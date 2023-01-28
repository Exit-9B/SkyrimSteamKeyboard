#pragma once

namespace Steam
{
	template <typename Param>
	class ICallbackHandler
	{
	public:
		virtual void HandleEvent(Param* a_param) = 0;
	};

	class CallbackManager final
	{
	public:
		static CallbackManager* GetSingleton();

		CallbackManager(const CallbackManager&) = delete;
		CallbackManager(CallbackManager&&) = delete;
		CallbackManager& operator=(const CallbackManager&) = delete;
		CallbackManager& operator=(CallbackManager&&) = delete;

		void RegisterListener(ICallbackHandler<::GamepadTextInputDismissed_t>* a_handler);

	private:
		CallbackManager();

		STEAM_CALLBACK(
			CallbackManager,
			OnGamepadTextInputDismissed,
			GamepadTextInputDismissed_t,
			_gamepadTextInputDismissedCallback);

		ICallbackHandler<::GamepadTextInputDismissed_t>* _textInputHandler = nullptr;
	};
}
