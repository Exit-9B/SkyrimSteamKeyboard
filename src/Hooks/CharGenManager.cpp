#include "CharGenManager.h"

#include "RE/Offset.h"
#include "Steam/Steam.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	CharGenManager* CharGenManager::GetSingleton()
	{
		static CharGenManager singleton{};
		return &singleton;
	}

	void CharGenManager::Install()
	{
		ShowTextEntryPatch();
		ShowVirtualKeyboardPatch();
	}

	void CharGenManager::SetDefaultName(std::string_view a_name)
	{
		_defaultName = a_name;
	}

	void CharGenManager::HandleEvent(::GamepadTextInputDismissed_t* a_param)
	{
		if (!a_param->m_bSubmitted) {
			return;
		}

		auto utils = ::SteamUtils();
		if (!utils) {
			return;
		}

		uint32 length = utils->GetEnteredGamepadTextLength();
		if (length == 0) {
			return;
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::RaceSexMenu>() : nullptr;
		const auto scaleformManager = RE::BSScaleformManager::GetSingleton();

		if (menu && scaleformManager) {
			auto buffer = std::make_unique<char[]>(length);
			utils->GetEnteredGamepadTextInput(buffer.get(), length);

			if (scaleformManager->IsValidName(buffer.get())) {
				menu->ChangeName(buffer.get());
			}
		}
	}

	void CharGenManager::ShowTextEntryPatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::RaceSexMenu::ShowTextEntry, 0x8E);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Failed to install CharGenManager::ShowTextEntryPatch");
		}

		auto& trampoline = SKSE::GetTrampoline();
		_AddArg = trampoline.write_call<5>(hook.address(), &AddArg);
	}

	void CharGenManager::ShowVirtualKeyboardPatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::ShowVirtualKeyboard);

		struct Patch : Xbyak::CodeGenerator
		{
			Patch(std::uintptr_t a_funcAddr)
			{
				mov(rax, a_funcAddr);
				jmp(rax);
			}
		};
		Patch patch{ reinterpret_cast<std::uintptr_t>(&TryShowGamepadTextInput) };
		patch.ready();

		REL::safe_write(hook.address(), patch.getCode(), patch.getSize());
	}

	void CharGenManager::TryShowGamepadTextInput()
	{
		if (!Steam::ShouldUseVirtualKeyboard()) {
			return;
		}

		SKSE::GetTaskInterface()->AddTask(&DoShowGamepadTextInput);
	}

	void CharGenManager::DoShowGamepadTextInput()
	{
		auto utils = ::SteamUtils();
		if (!utils) {
			return;
		}

		auto charGenManager = GetSingleton();

		const auto player = RE::PlayerCharacter::GetSingleton();
		const auto playerBase = player ? player->GetActorBase() : nullptr;
		const char* playerName = playerBase ? playerBase->fullName.c_str() : nullptr;

		auto& defaultName = charGenManager->_defaultName;
		bool hasCustomName = playerName && ::strcmp(playerName, defaultName.c_str()) != 0;
		const char* existingText = hasCustomName ? playerName : nullptr;

		utils->ShowGamepadTextInput(
			k_EGamepadTextInputModeNormal,
			k_EGamepadTextInputLineModeSingleLine,
			nullptr,
			NAME_MAX_CHARS,
			existingText);

		Steam::CallbackManager::GetSingleton()->RegisterListener(charGenManager);
	}

	void CharGenManager::AddArg(RE::GArray<RE::GFxValue>* a_args, RE::GFxValue& a_bShowTextEntry)
	{
		bool showTextEntry = !Steam::ShouldUseVirtualKeyboard();
		a_bShowTextEntry.SetBoolean(showTextEntry);
		_AddArg(a_args, a_bShowTextEntry);
	}
}
