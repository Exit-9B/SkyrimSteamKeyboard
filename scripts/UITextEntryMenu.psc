Scriptname UITextEntryMenu extends UIMenuBase

string property		ROOT_MENU		= "CustomMenu" autoReadonly
string Property 	MENU_ROOT		= "_root.textEntry." autoReadonly

string _internalString = ""
string _internalResult = ""

string Function GetMenuName()
	return "UITextEntryMenu"
EndFunction

string Function GetResultString()
	return _internalResult
EndFunction

Function SetPropertyString(string propertyName, string value)
	If propertyName == "text"
		_internalString = value
	Endif
EndFunction

Function ResetMenu()
	isResetting = true
	_internalString = ""
	_internalResult = ""
	isResetting = false
EndFunction

int Function OpenMenu(Form inForm = None, Form akReceiver = None)
	_internalResult = ""

	If !BlockUntilClosed() || !WaitForReset()
		return 0
	Endif

	If VirtualKeyboard.ShouldBeUsed()
		RegisterForModEvent("VirtualKeyboardClose", "OnVirtualKeyboardClose")

		Lock()
		VirtualKeyboard.Show("", _internalString)
		If !WaitLock()
			return 0
		EndIf
	Else
		RegisterForModEvent("UITextEntryMenu_LoadMenu", "OnLoadMenu")
		RegisterForModEvent("UITextEntryMenu_CloseMenu", "OnUnloadMenu")
		RegisterForModEvent("UITextEntryMenu_TextChanged", "OnTextChanged")

		Lock()
		UI.OpenCustomMenu("textentrymenu")
		If !WaitLock()
			return 0
		Endif
	Endif

	return 1
EndFunction

Event OnLoadMenu(string eventName, string strArg, float numArg, Form formArg)
	UpdateTextEntryString()
EndEvent

Event OnUnloadMenu(string eventName, string strArg, float numArg, Form formArg)
	UnregisterForModEvent("UITextEntryMenu_LoadMenu")
	UnregisterForModEvent("UITextEntryMenu_CloseMenu")
	UnregisterForModEvent("UITextEntryMenu_TextChanged")
EndEvent

Event OnTextChanged(string eventName, string strArg, float numArg, Form formArg)
	_internalResult = strArg
	Unlock()
EndEvent

Event OnVirtualKeyboardClose(string eventName, string strArg, float numArg, Form formArg)
	_internalResult = strArg
	UnregisterForModEvent("VirtualKeyboardClose")
	Unlock()
EndEvent

Function UpdateTextEntryString()
	UI.InvokeString(ROOT_MENU, MENU_ROOT + "setTextEntryMenuText", _internalString)
EndFunction

