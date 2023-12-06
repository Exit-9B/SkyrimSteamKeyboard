#pragma once

namespace Papyrus
{
	namespace VirtualKeyboard
	{
		inline constexpr std::string_view SCRIPT_NAME = "VirtualKeyboard"sv;

		bool ShouldBeUsed(RE::StaticFunctionTag*);

		void Show(
			RE::StaticFunctionTag*,
			std::string asDescription,
			std::string asExistingText,
			std::uint32_t aiMaxChars,
			bool abMultipleLines,
			bool abPassword);

		void VirtualKeyboardDone(void* a_userParam, const char* a_text);

		void OnVirtualKeyboardCancel();

		bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm);
	}
}
