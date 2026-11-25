# Game Vault (Xbox): like x64-windows-release, but SDL3 as a DLL, so the Xbox app can ship our Xbox SDL3 (sdl3-uwp) in its place
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_BUILD_TYPE release)
if(PORT STREQUAL "sdl3")
    set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()
