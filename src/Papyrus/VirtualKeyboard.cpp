#include "VirtualKeyboard.h"

#include "EventDispatcher.h"
#include "Steam/Steam.h"

#define REGISTER(a_vm, a_function) \
	a_vm->RegisterFunction(#a_function##sv, SCRIPT_NAME, &a_function)

namespace Papyrus
{
	bool VirtualKeyboard::ShouldBeUsed(RE::StaticFunctionTag*)
	{
		return Steam::ShouldUseVirtualKeyboard();
	}

	void VirtualKeyboard::Show(
		RE::StaticFunctionTag*,
		std::string asDescription,
		std::string asExistingText,
		std::uint32_t aiMaxChars,
		bool abMultipleLines,
		bool abPassword)
	{
		auto utils = ::SteamUtils();
		if (!utils || !utils->IsOverlayEnabled()) {
			return;
		}

		auto inputMode = !abPassword
			? k_EGamepadTextInputModeNormal
			: k_EGamepadTextInputModePassword;

		auto lineInputMode = !abMultipleLines
			? k_EGamepadTextInputLineModeSingleLine
			: k_EGamepadTextInputLineModeMultipleLines;

		utils->ShowGamepadTextInput(
			inputMode,
			lineInputMode,
			asDescription.c_str(),
			aiMaxChars,
			asExistingText.c_str());

		Steam::CallbackManager::GetSingleton()->RegisterListener(EventDispatcher::GetSingleton());
	}

	bool VirtualKeyboard::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
	{
		REGISTER(a_vm, ShouldBeUsed);
		REGISTER(a_vm, Show);

		return true;
	}
}
