#include "MainThreadManager.h"

#include "RE/Offset.h"

namespace Hooks
{
	MainThreadManager* MainThreadManager::GetSingleton()
	{
		static MainThreadManager singleton{};
		return &singleton;
	}

	void MainThreadManager::Install()
	{
		auto hook = REL::Relocation<std::uintptr_t>(RE::Offset::Main::Update, 0x3E);

		if (!REL::make_pattern<"E8">().match(hook.address())) {
			util::report_and_fail("Failed to install MainThreadManager");
		}

		auto& trampoline = SKSE::GetTrampoline();
		_DoFrame = trampoline.write_call<5>(hook.address(), &OnFrame);
	}

	void MainThreadManager::OnFrame(RE::Main* a_main)
	{
		::SteamAPI_RunCallbacks();
		return _DoFrame(a_main);
	}
}
