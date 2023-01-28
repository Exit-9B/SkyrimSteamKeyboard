#pragma once

#include "Steam/CallbackManager.h"

namespace Hooks
{
	class CharGenManager final : public Steam::ICallbackHandler<::GamepadTextInputDismissed_t>
	{
	public:
		static CharGenManager* GetSingleton();

		CharGenManager(const CharGenManager&) = delete;
		CharGenManager(CharGenManager&&) = delete;
		CharGenManager& operator=(const CharGenManager&) = delete;
		CharGenManager& operator=(CharGenManager&&) = delete;

		void Install();

		void SetDefaultName(std::string_view a_name);

		void HandleEvent(::GamepadTextInputDismissed_t* a_param) override;

	private:
		inline static constexpr uint32 NAME_MAX_CHARS = 26;

		CharGenManager() = default;

		void ShowTextEntryPatch();

		void ShowVirtualKeyboardPatch();

		static void TryShowGamepadTextInput();

		static void DoShowGamepadTextInput();

		static void AddArg(RE::GArray<RE::GFxValue>* a_args, RE::GFxValue& a_bShowTextEntry);

		inline static REL::Relocation<decltype(&AddArg)> _AddArg;

		std::string _defaultName;
	};
}
