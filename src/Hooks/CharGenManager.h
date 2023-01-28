#pragma once

namespace Hooks
{
	class CharGenManager final
	{
	public:
		static CharGenManager* GetSingleton();

		CharGenManager(const CharGenManager&) = delete;
		CharGenManager(CharGenManager&&) = delete;
		CharGenManager& operator=(const CharGenManager&) = delete;
		CharGenManager& operator=(CharGenManager&&) = delete;

		void Install();

		void SetDefaultName(std::string_view a_name);

	private:
		inline static constexpr uint32 NAME_MAX_CHARS = 26;

		CharGenManager();

		void InstallShowTextEntryOverride();

		void InstallShowVirtualKeyboard();

		static void TryShowGamepadTextInput();

		static void DoShowGamepadTextInput();

		static void AddArg(RE::GArray<RE::GFxValue>* a_args, RE::GFxValue& a_bShowTextEntry);

		STEAM_CALLBACK(
			CharGenManager,
			OnGamepadTextInputDismissed,
			GamepadTextInputDismissed_t,
			_gamepadTextInputDismissedCallback);

		inline static REL::Relocation<decltype(&AddArg)> _AddArg;

		inline static std::string _defaultName;
	};
}
