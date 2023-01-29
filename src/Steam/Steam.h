#pragma once

namespace Steam
{
	bool IsBigPictureEnabled();

	bool IsOverlayEnabled();

	bool IsUsingGamepad();

	bool ShouldUseVirtualKeyboard();

	std::string GetTextInput(::GamepadTextInputDismissed_t* a_param);
}
