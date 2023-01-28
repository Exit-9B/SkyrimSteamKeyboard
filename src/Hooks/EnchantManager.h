#pragma once

#include "Steam/CallbackManager.h"

namespace Hooks
{
	class EnchantManager final : public Steam::ICallbackHandler<::GamepadTextInputDismissed_t>
	{
	public:
		static EnchantManager* GetSingleton();

		void Install();

		EnchantManager(const EnchantManager&) = delete;
		EnchantManager(EnchantManager&&) = delete;
		EnchantManager& operator=(const EnchantManager&) = delete;
		EnchantManager& operator=(EnchantManager&&) = delete;

		void HandleEvent(::GamepadTextInputDismissed_t* a_param) override;

	private:
		EnchantManager() = default;

		void EditItemNamePatch();

		static void TryShowGamepadTextInput(const char* a_itemName);

		static void DoShowGamepadTextInput(const std::string& a_existingText);

		static void EditItemName(
			RE::CraftingSubMenus::EnchantConstructMenu* a_menu,
			RE::UserEvents* a_userEvents);

		inline static REL::Relocation<decltype(&EditItemName)> _EditItemName;
	};
}
