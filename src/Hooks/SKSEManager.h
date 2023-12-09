#pragma once

namespace Hooks
{
	class SKSEManager final
	{
	public:
		static SKSEManager* GetSingleton();

		SKSEManager(const SKSEManager&) = delete;
		SKSEManager(SKSEManager&&) = delete;
		SKSEManager& operator=(const SKSEManager&) = delete;
		SKSEManager& operator=(SKSEManager&&) = delete;

		void Install();

		void SetMovie(RE::GFxMovie* a_movie);

		static void VirtualKeyboardDone(void* a_userParam, const char* a_text);

		static void OnVirtualKeyboardCancel();

	private:
		SKSEManager() = default;

		void VirtualKeyboardDismissed(bool a_submitted, const char* a_text);

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
