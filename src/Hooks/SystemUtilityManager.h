#pragma once

namespace Hooks
{
	class SystemUtilityManager final
	{
	public:
		using DoneCallback = void(void*, const char*);
		using CancelCallback = void();

		[[nodiscard]] static SystemUtilityManager* GetSingleton();

		SystemUtilityManager(const SystemUtilityManager&) = delete;
		SystemUtilityManager(SystemUtilityManager&&) = delete;

		~SystemUtilityManager() = default;

		SystemUtilityManager& operator=(const SystemUtilityManager&) = delete;
		SystemUtilityManager& operator=(SystemUtilityManager&&) = delete;

		void Install();

		bool ShouldUseVirtualKeyboard() const;

		void ShowVirtualKeyboard(
			const char* a_existingText,
			DoneCallback* a_doneCallback,
			CancelCallback* a_cancelCallback,
			void* a_userParam,
			std::uint32_t a_maxChars,
			const char* a_description = nullptr,
			bool a_multipleLines = false,
			bool a_password = false);

		static void DoInitialize(RE::BSWin32SystemUtility* a_systemUtility);

		static void PostInitialize(RE::BSWin32SystemUtility* a_systemUtility);

		static void StartInputProcessing(
			RE::BSInputDeviceManager* a_inputDeviceManager,
			const RE::InputEvent** a_first);

	private:
		SystemUtilityManager() = default;

		inline static REL::Relocation<decltype(&DoInitialize)> _DoInitialize;
		inline static REL::Relocation<decltype(&StartInputProcessing)> _StartInputProcessing;

		inline static bool IsRunningOnSteamDeck = false;
		inline static bool IsBigPictureEnabled = false;
	};
}
