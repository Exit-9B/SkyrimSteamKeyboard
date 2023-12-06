#include "SystemUtilityManager.h"

#include "RE/Offset.h"

#include <steam/isteamutils.h>

#include <Windows.h>

#include <tchar.h>

namespace Hooks
{
	SystemUtilityManager* SystemUtilityManager::GetSingleton()
	{
		static SystemUtilityManager singleton{};
		return &singleton;
	}

	void SystemUtilityManager::Install()
	{
		auto vtbl = REL::Relocation<std::uintptr_t>(RE::Offset::BSWin32SystemUtility::Vtbl);
		_DoInitialize = vtbl.write_vfunc(4, &DoInitialize);

		auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::BSInputDeviceManager::PollInputDevices,
			0x7B);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Failed to install SystemUtilityManager");
		}

		auto& trampoline = SKSE::GetTrampoline();
		_StartInputProcessing = trampoline.write_call<5>(hook.address(), &StartInputProcessing);
	}

	static bool IsUsingGamepad(RE::BSInputDeviceManager* inputDeviceManager)
	{
		using IsUsingGamepad_t = bool (*)(RE::BSInputDeviceManager*);
		REL::Relocation<IsUsingGamepad_t> isUsingGamepad{
			RE::Offset::BSInputDeviceManager::IsUsingGamepad
		};

		return isUsingGamepad(inputDeviceManager);
	}

	static bool IsOverlayEnabled()
	{
		const auto systemUtility = RE::BSWin32SystemUtility::GetSingleton();
		const auto isteamutils = systemUtility ? systemUtility->steamUtils : nullptr;
		return isteamutils ? isteamutils->IsOverlayEnabled() : false;
	}

	bool SystemUtilityManager::ShouldUseVirtualKeyboard() const
	{
		const auto inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
		return inputDeviceManager && inputDeviceManager->GetVirtualKeyboard() &&
			(IsRunningOnSteamDeck ||
			 (IsBigPictureEnabled && IsOverlayEnabled() && IsUsingGamepad(inputDeviceManager)));
	}

	void SystemUtilityManager::ShowVirtualKeyboard(
		const char* a_existingText,
		DoneCallback* a_doneCallback,
		CancelCallback* a_cancelCallback,
		void* a_userParam,
		std::uint32_t a_maxChars,
		const char* a_description,
		bool a_multipleLines,
		bool a_password)
	{
		const auto systemUtility = RE::BSWin32SystemUtility::GetSingleton();
		const auto isteamutils = systemUtility ? systemUtility->steamUtils : nullptr;
		if (!isteamutils) {
			return;
		}

		systemUtility->virtualKeyboardDone = a_doneCallback;
		systemUtility->virtualKeyboardCancel = a_cancelCallback;
		systemUtility->virtualKeyboardUserParam = a_userParam;

		const auto inputMode = !a_password
			? k_EGamepadTextInputModeNormal
			: k_EGamepadTextInputModePassword;

		const auto lineInputMode = !a_multipleLines
			? k_EGamepadTextInputLineModeSingleLine
			: k_EGamepadTextInputLineModeMultipleLines;

		isteamutils->ShowGamepadTextInput(
			inputMode,
			lineInputMode,
			a_description,
			a_maxChars,
			a_existingText);
	}

	void SystemUtilityManager::DoInitialize(RE::BSWin32SystemUtility* a_systemUtility)
	{
		_DoInitialize(a_systemUtility);
		PostInitialize(a_systemUtility);
	}

	static bool IsSteamInBigPictureMode()
	{
		const auto bufSize = ::GetEnvironmentVariable(TEXT("SteamTenfoot"), nullptr, 0);
		if (!bufSize) {
			return false;
		}

		const auto buffer = std::make_unique<::TCHAR[]>(bufSize);
		::GetEnvironmentVariable(TEXT("SteamTenfoot"), buffer.get(), bufSize);

		return ::_tcscmp(buffer.get(), TEXT("1")) == 0;
	}

	void SystemUtilityManager::PostInitialize(RE::BSWin32SystemUtility* a_systemUtility)
	{
		assert(a_systemUtility);

		const auto isteamutils = a_systemUtility->steamUtils;
		IsRunningOnSteamDeck = isteamutils && isteamutils->IsSteamRunningOnSteamDeck();
		IsBigPictureEnabled = IsSteamInBigPictureMode();
	}

	void SystemUtilityManager::StartInputProcessing(
		RE::BSInputDeviceManager* a_inputDeviceManager,
		const RE::InputEvent** a_first)
	{
		_StartInputProcessing(a_inputDeviceManager, a_first);

		const auto systemUtility = RE::BSWin32SystemUtility::GetSingleton();
		if (systemUtility) {
			systemUtility->isRunningOnSteamDeck = GetSingleton()->ShouldUseVirtualKeyboard();
		}
	}
}
