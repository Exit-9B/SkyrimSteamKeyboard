#include "EnchantManager.h"

#include "RE/Offset.h"
#include "Steam/Steam.h"

namespace Hooks
{
	EnchantManager* EnchantManager::GetSingleton()
	{
		static EnchantManager singleton{};
		return &singleton;
	}

	void EnchantManager::Install()
	{
		EditItemNamePatch();
	}

	void EnchantManager::HandleEvent(::GamepadTextInputDismissed_t* a_param)
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
		const auto menu = ui ? ui->GetMenu<RE::CraftingMenu>() : nullptr;
		const auto subMenu = menu
			? skyrim_cast<RE::CraftingSubMenus::EnchantConstructMenu*>(menu->subMenu)
			: nullptr;
		const auto scaleformManager = RE::BSScaleformManager::GetSingleton();

		if (!subMenu || !scaleformManager) {
			return;
		}

		auto buffer = std::make_unique<char[]>(length);
		utils->GetEnteredGamepadTextInput(buffer.get(), length);

		SKSE::GetTaskInterface()->AddUITask(
			[=, name = std::string(buffer.get())]
			{
				if (scaleformManager->IsValidName(name.c_str())) {
					subMenu->RenameItem(name.c_str());
				}

				subMenu->UpdateInterface();
			});
	}

	void EnchantManager::EditItemNamePatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::EnchantConstructMenu::ProcessUserEvent,
			0x2BD);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Failed to install EnchantManager::EditItemNamePatch");
		}

		auto& trampoline = SKSE::GetTrampoline();
		_EditItemName = trampoline.write_call<5>(hook.address(), &EditItemName);
	}

	void EnchantManager::TryShowGamepadTextInput(const char* a_itemName)
	{
		if (!Steam::IsBigPictureEnabled()) {
			return;
		}

		SKSE::GetTaskInterface()->AddTask(
			[name = std::string(a_itemName)]
			{
				DoShowGamepadTextInput(name);
			});
	}

	void EnchantManager::DoShowGamepadTextInput(const std::string& a_existingText)
	{
		auto utils = ::SteamUtils();
		if (!utils || !utils->IsOverlayEnabled()) {
			return;
		}

		auto enchantManager = GetSingleton();

		static auto uMaxCustomItemNameLength =
			RE::INISettingCollection::GetSingleton()->GetSetting(
				"uMaxCustomItemNameLength:Interface");
		uint32 maxLength = uMaxCustomItemNameLength ? uMaxCustomItemNameLength->GetUInt() : 32;

		utils->ShowGamepadTextInput(
			k_EGamepadTextInputModeNormal,
			k_EGamepadTextInputLineModeSingleLine,
			nullptr,
			maxLength,
			a_existingText.c_str());

		Steam::CallbackManager::GetSingleton()->RegisterListener(enchantManager);
	}

	void EnchantManager::EditItemName(
		RE::CraftingSubMenus::EnchantConstructMenu* a_menu,
		RE::UserEvents* a_userEvents)
	{
		if (!Steam::ShouldUseVirtualKeyboard()) {
			return _EditItemName(a_menu, a_userEvents);
		}

		if (a_menu->sliderShown) {
			return;
		}

		auto& itemPreview = a_menu->craftItemPreview;
		auto displayName = itemPreview ? itemPreview->GetDisplayName() : nullptr;

		SKSE::GetTaskInterface()->AddTask(
			[name = std::string(displayName)]
			{
				DoShowGamepadTextInput(name);
			});
	}
}
