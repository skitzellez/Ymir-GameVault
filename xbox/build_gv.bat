@echo off
rem Game Vault · Ymir: desktop MSVC build (the Xbox app wraps this exe). Usage: xbox\build_gv.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1
cd /d "%~dp0.."
if not exist vcpkg\vcpkg.exe call vcpkg\bootstrap-vcpkg.bat -disableMetrics || exit /b 1
cmake -S . -B build-gv -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-xbox-gv -DVCPKG_OVERLAY_TRIPLETS=xbox/triplets ^
  -DYmir_ENABLE_TESTS=OFF -DYmir_ENABLE_SANDBOX=OFF -DYmir_ENABLE_YMDASM=OFF ^
  -DYmir_ENABLE_IMGUI_DEMO=OFF -DYmir_ENABLE_UPDATE_CHECKS=OFF -DYmir_ENABLE_DEVLOG=OFF -DYmir_AVX2=ON || exit /b 1
cmake --build build-gv --target ymir-sdl3 || exit /b 1
