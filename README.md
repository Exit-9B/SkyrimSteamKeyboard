Allows Skyrim to make use of Steam Big Picture functionality.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [SKSE64 SDK](https://skse.silverlock.org/)
	* Add the environment variable `SKSE64Path`
* [Steamworks SDK v1.55](https://partner.steamgames.com/downloads/list)
	* Extract to `external/steamworks_sdk_155`

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/Exit-9B/SkyrimSteamKeyboard
cd SkyrimSteamKeyboard
git submodule init
git submodule update
cmake --preset vs2022-windows
cmake --build build --config Release
```
