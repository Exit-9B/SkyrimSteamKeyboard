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
		if (!IsBigPictureEnabled()) {
			return false;
		}

		if (!IsUsingGamepad()) {
			return false;
		}

		auto utils = ::SteamUtils();
		if (!utils || !utils->IsOverlayEnabled()) {
			return false;
		}

		return true;
	}
}
