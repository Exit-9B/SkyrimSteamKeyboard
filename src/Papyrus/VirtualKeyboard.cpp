#include "VirtualKeyboard.h"

#include "Hooks/SystemUtilityManager.h"

#define REGISTER(a_vm, a_function) \
	a_vm->RegisterFunction(#a_function##sv, SCRIPT_NAME, &a_function)

namespace Papyrus
{
	bool VirtualKeyboard::ShouldBeUsed(RE::StaticFunctionTag*)
	{
		return Hooks::SystemUtilityManager::GetSingleton()->ShouldUseVirtualKeyboard();
	}

	void VirtualKeyboard::Show(
		RE::StaticFunctionTag*,
		std::string asDescription,
		std::string asExistingText,
		std::uint32_t aiMaxChars,
		bool abMultipleLines,
		bool abPassword)
	{
		Hooks::SystemUtilityManager::GetSingleton()->ShowVirtualKeyboard(
			asExistingText.c_str(),
			&VirtualKeyboardDone,
			&OnVirtualKeyboardCancel,
			nullptr,
			aiMaxChars,
			asDescription.c_str(),
			abMultipleLines,
			abPassword);
	}

	void VirtualKeyboard::VirtualKeyboardDone(
		[[maybe_unused]] void* a_userParam,
		const char* a_text)
	{
		auto modEvent = SKSE::ModCallbackEvent{
			.eventName = "VirtualKeyboardClose",
			.strArg = a_text,
			.numArg = 0.0f,
			.sender = nullptr,
		};

		auto eventSource = SKSE::GetModCallbackEventSource();
		eventSource->SendEvent(&modEvent);
	}

	void VirtualKeyboard::OnVirtualKeyboardCancel()
	{
		auto modEvent = SKSE::ModCallbackEvent{
			.eventName = "VirtualKeyboardClose",
			.strArg = nullptr,
			.numArg = 1.0f,
			.sender = nullptr,
		};

		auto eventSource = SKSE::GetModCallbackEventSource();
		eventSource->SendEvent(&modEvent);
	}

	bool VirtualKeyboard::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
	{
		REGISTER(a_vm, ShouldBeUsed);
		REGISTER(a_vm, Show);

		return true;
	}
}
