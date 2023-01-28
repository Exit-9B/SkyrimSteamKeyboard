Scriptname VirtualKeyboard Hidden

; These functions are provided for integration in SkyUILib and UIExtensions.
; You should not need to use them in your mods.

; Get whether we should use the virtual keyboard (i.e. Big Picture is available and the player is using the gamepad).
bool Function ShouldBeUsed() global native

; Shows the virtual keyboard, and raises the mod event "VirtualKeyboardDismissed" when it is dismissed.
Function Show(string asDescription = "", string asExistingText = "", int aiMaxChars = 32, bool abMultipleLines = false, bool abPassword = false) global native
