// Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
// Game Vault (Xbox): helpers for running Ymir as an Xbox Dev Mode app (our sdl3-uwp SDL3.dll in the package).
#pragma once

#include <functional>
#include <string>

namespace gv {

// True inside an app package (the Xbox app); false for the normal PC build.
bool IsXboxApp();

// Appends a line to <LocalState>\Ymir\xbox_start.txt (only in the Xbox app).
void XboxLog(const std::string &line);
void XboxLogReset();

// Sound for the Xbox app: SDL audio devices hang inside an app package, so an XAudio2 voice plays
// 16-bit stereo; `pull` fills the given number of stereo frames.
class XboxAudio {
public:
    bool Start(int sampleRate, std::function<void(short *frames, int count)> pull);
    void Stop();
    void SetVolume(float volume);
    ~XboxAudio() {
        Stop();
    }

private:
    struct Impl;
    Impl *m_impl = nullptr;
};

} // namespace gv
