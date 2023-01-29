#include "Steam.h"

#include "RE/Offset.h"

#include <Windows.h>

#include <tchar.h>

namespace Steam
{
	bool IsBigPictureEnabled()
	{
		const auto bufSize = ::GetEnvironmentVariable(TEXT("SteamTenfoot"), nullptr, 0);
		if (!bufSize) {
			return false;
		}

		const auto buffer = std::make_unique<::TCHAR[]>(bufSize);
		::GetEnvironmentVariable(TEXT("SteamTenfoot"), buffer.get(), bufSize);

		return ::_tcscmp(buffer.get(), TEXT("1")) == 0;
	}

	bool IsOverlayEnabled()
	{
		auto utils = ::SteamUtils();
		return utils && utils->IsOverlayEnabled();
	}

	bool IsUsingGamepad()
	{
		using IsUsingGamepad_t = bool (*)(RE::BSInputDeviceManager*);
		REL::Relocation<IsUsingGamepad_t> isUsingGamepad{
			RE::Offset::BSInputDeviceManager::IsUsingGamepad
		};

		const auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		return deviceManager && isUsingGamepad(deviceManager);
	}

	bool ShouldUseVirtualKeyboard()
	{
		return IsBigPictureEnabled() && IsUsingGamepad() && IsOverlayEnabled();
	}

	std::string GetTextInput(::GamepadTextInputDismissed_t* a_param)
	{
		std::string textInput;

		if (!a_param->m_bSubmitted) {
			return textInput;
		}

		auto utils = ::SteamUtils();
		if (!utils) {
			return textInput;
		}

		uint32 length = utils->GetEnteredGamepadTextLength();
		if (length == 0) {
			return textInput;
		}

		auto buffer = std::make_unique<char[]>(length);
		utils->GetEnteredGamepadTextInput(buffer.get(), length);

		textInput = buffer.get();

		return textInput;
	}
}
