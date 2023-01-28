#include "Steam.h"

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

	bool ShouldUseVirtualKeyboard()
	{
		if (!IsBigPictureEnabled()) {
			return false;
		}

		auto utils = ::SteamUtils();
		if (!utils || !utils->IsOverlayEnabled()) {
			return false;
		}

		return true;
	}
}
