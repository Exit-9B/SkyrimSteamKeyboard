#pragma once

#include "Steam/CallbackManager.h"

namespace Hooks
{
	class SKSEManager final : public Steam::ICallbackHandler<::GamepadTextInputDismissed_t>
	{
	public:
		static SKSEManager* GetSingleton();

		SKSEManager(const SKSEManager&) = delete;
		SKSEManager(SKSEManager&&) = delete;
		SKSEManager& operator=(const SKSEManager&) = delete;
		SKSEManager& operator=(SKSEManager&&) = delete;

		void Install();

		void HandleEvent(::GamepadTextInputDismissed_t* a_param) override;

		void SetMovie(RE::GFxMovie* a_movie);

	private:
		SKSEManager() = default;

		void ScaleformPatch();

		static void RegisterScaleformFunctions(
			RE::GFxMovieView* a_view,
			RE::GFxMovieView::ScaleModeType a_scaleMode);

		inline static REL::Relocation<decltype(&RegisterScaleformFunctions)> _SetViewScaleMode;

		RE::GFxMovie* _movie = nullptr;
	};

	class SKSEScaleform_AllowTextInput : public RE::GFxFunctionHandler
	{
	public:
		void Call(Params& a_params) override;
	};
}
