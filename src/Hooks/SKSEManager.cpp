#include "SKSEManager.h"

#include "Steam/Steam.h"

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

	void SKSEManager::HandleEvent(::GamepadTextInputDismissed_t* a_param)
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
		if (a_param->m_bSubmitted) {
			RE::GFxValue text;
			std::string input = Steam::GetTextInput(a_param);
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
			args[1].SetNumber(a_param->m_bSubmitted ? Accept : Cancel);
			args[2].SetNumber(0);
			args[3].SetString(a_param->m_bSubmitted ? "Accept" : "Cancel");
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

	void SKSEScaleform_AllowTextInput::Call(Params& a_params)
	{
		if (a_params.argCount < 1) {
			return;
		}

		const bool enable = a_params.args[0].GetBool();

		if (enable && Steam::ShouldUseVirtualKeyboard()) {
			auto utils = ::SteamUtils();
			if (!utils) {
				return;
			}

			::EGamepadTextInputMode inputMode = k_EGamepadTextInputModeNormal;
			::EGamepadTextInputLineMode lineInputMode = k_EGamepadTextInputLineModeSingleLine;
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
				inputMode = k_EGamepadTextInputModePassword;
			}

			RE::GFxValue multiline;
			textField.GetMember("multiline", &multiline);
			if (multiline.IsBool() && multiline.GetBool() == true) {
				lineInputMode = k_EGamepadTextInputLineModeMultipleLines;
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

			utils->ShowGamepadTextInput(
				inputMode,
				lineInputMode,
				nullptr,
				charMax,
				existingText.c_str());

			auto skseManager = SKSEManager::GetSingleton();
			skseManager->SetMovie(a_params.movie);
			Steam::CallbackManager::GetSingleton()->RegisterListener(skseManager);
		}
		else {
			if (const auto controlMap = RE::ControlMap::GetSingleton()) {
				controlMap->AllowTextInput(enable);
			}
		}
	}
}
