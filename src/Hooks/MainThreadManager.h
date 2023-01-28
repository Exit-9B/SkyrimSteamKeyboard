#pragma once

namespace Hooks
{
	class MainThreadManager final
	{
	public:
		static MainThreadManager* GetSingleton();

		MainThreadManager(const MainThreadManager&) = delete;
		MainThreadManager(MainThreadManager&&) = delete;
		MainThreadManager& operator=(const MainThreadManager&) = delete;
		MainThreadManager& operator=(MainThreadManager&&) = delete;

		void Install();

	private:
		MainThreadManager() = default;

		static void OnFrame(RE::Main* a_main);

		inline static REL::Relocation<decltype(&OnFrame)> _DoFrame;
	};
}
