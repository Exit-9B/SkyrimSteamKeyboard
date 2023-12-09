#include "SKSEManager.h"

#include "SystemUtilityManager.h"

namespace Hooks
{
	SKSEManager* SKSEManager::GetSingleton()
	{
		static SKSEManager singleton{};
		return &singleton;
	}

	void SKSEManager::Install()
	{
		ScaleformPatch();
	}

	void SKSEManager::VirtualKeyboardDismissed(bool a_submitted, const char* a_text)
	{
		if (!_movie) {
			return;
		}

		RE::GFxValue focus;
		_movie->InvokeFmt("Selection.getFocus", &focus, "");

		RE::GFxValue textField;
		_movie->GetVariable(&textField, focus.GetString());

		if (!textField.IsObject()) {
			logger::error(
				"Failed to find focused text field ({}), not submitting input",
				focus.GetString());

			return;
		}

		// Only alter the text field ourselves if text was submitted
		if (a_submitted) {
			RE::GFxValue text;
			std::string input = a_text;
			text.SetString(input);

			textField.SetMember("text", text);

			RE::GFxValue textInput;
			textField.GetMember("_parent", &textInput);
			if (textInput.IsObject() && textInput.HasMember("_text")) {
				textInput.SetMember("_text", text);
			}
		}

		RE::GFxValue inputDelegate;
		_movie->GetVariable(&inputDelegate, "gfx.managers.InputDelegate._instance");
		if (inputDelegate.IsObject()) {
			enum InputCode
			{
				Accept = 13,
				Cancel = 9,
			};

			std::array<RE::GFxValue, 5> args;
			args[0].SetString("keyDown");
			args[1].SetNumber(a_submitted ? Accept : Cancel);
			args[2].SetNumber(0);
			args[3].SetString(a_submitted ? "Accept" : "Cancel");
			args[4].SetNull();

			inputDelegate.Invoke("handleKeyPress", args);
		}
		else {
			logger::error("Failed to find InputDelegate, unfocusing text field directly");

			RE::GFxValue null;
			null.SetNull();
			_movie->Invoke("Selection.setFocus", nullptr, &null, 1);
		}
	}

	void SKSEManager::SetMovie(RE::GFxMovie* a_movie)
	{
		_movie = a_movie;
	}

	void SKSEManager::ScaleformPatch()
	{
		auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::BSScaleformManager::LoadMovie,
			0x1DD);

		// SKSE should write these bytes
		if (!REL::make_pattern<"FF 15">().match(hook.address())) {
			util::report_and_fail("Failed to install SKSEManager::ScaleformPatch");
		}

		auto& trampoline = SKSE::GetTrampoline();
		auto ptr = trampoline.write_call<6>(hook.address(), &RegisterScaleformFunctions);
		_SetViewScaleMode = *reinterpret_cast<std::uintptr_t*>(ptr);
	}

	void SKSEManager::RegisterScaleformFunctions(
		RE::GFxMovieView* a_view,
		RE::GFxMovieView::ScaleModeType a_scaleMode)
	{
		// SKSE inserts its Scaleform functions
		_SetViewScaleMode(a_view, a_scaleMode);

		logger::debug("Overriding SKSE Scaleform functions");

		RE::GFxValue skse;
		a_view->GetVariable(&skse, "_global.skse");

		if (!skse.IsObject()) {
			logger::error("Failed to get _global.skse object");
			return;
		}

		RE::GFxValue fn_AllowTextInput;
		static auto AllowTextInput = new SKSEScaleform_AllowTextInput;
		a_view->CreateFunction(&fn_AllowTextInput, AllowTextInput);
		skse.SetMember("AllowTextInput", fn_AllowTextInput);
	}

	void SKSEManager::VirtualKeyboardDone([[maybe_unused]] void* a_userParam, const char* a_text)
	{
		GetSingleton()->VirtualKeyboardDismissed(true, a_text);
	}

	void SKSEManager::OnVirtualKeyboardCancel()
	{
		GetSingleton()->VirtualKeyboardDismissed(false, "");
	}

	void SKSEScaleform_AllowTextInput::Call(Params& a_params)
	{
		if (a_params.argCount < 1) {
			return;
		}

		const bool enable = a_params.args[0].GetBool();

		if (enable && SystemUtilityManager::GetSingleton()->ShouldUseVirtualKeyboard()) {
			bool inputPassword = false;
			bool inputMultipleLines = false;
			uint32 charMax = 65532;
			std::string existingText;

			RE::GFxValue focus;
			a_params.movie->InvokeFmt("Selection.getFocus", &focus, "");

			RE::GFxValue textField;
			a_params.movie->GetVariable(&textField, focus.GetString());

			if (!textField.IsObject()) {
				logger::error(
					"Failed to find focused text field ({}), not showing virtual keyboard",
					focus.GetString());

				return;
			}

			RE::GFxValue password;
			textField.GetMember("password", &password);
			if (password.IsBool() && password.GetBool() == true) {
				inputPassword = true;
			}

			RE::GFxValue multiline;
			textField.GetMember("multiline", &multiline);
			if (multiline.IsBool() && multiline.GetBool() == true) {
				inputMultipleLines = true;
			}

			RE::GFxValue maxChars;
			textField.GetMember("maxChars", &maxChars);
			if (maxChars.IsNumber()) {
				charMax = static_cast<uint32>(maxChars.GetNumber());
			}

			RE::GFxValue text;
			textField.GetMember("text", &text);
			if (text.IsString()) {
				existingText = text.GetString();
			}

			auto skseManager = SKSEManager::GetSingleton();
			skseManager->SetMovie(a_params.movie);

			Hooks::SystemUtilityManager::GetSingleton()->ShowVirtualKeyboard(
				existingText.c_str(),
				&SKSEManager::VirtualKeyboardDone,
				&SKSEManager::OnVirtualKeyboardCancel,
				nullptr,
				charMax,
				nullptr,
				inputMultipleLines,
				inputPassword);
		}
		else {
			if (const auto controlMap = RE::ControlMap::GetSingleton()) {
				controlMap->AllowTextInput(enable);
			}
		}
	}
}
