// Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
// PolyForm Noncommercial License 1.0.0 for this file's Game Vault parts; Ymir itself is GPLv3.
#include "gv_xbox.hpp"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
    #include <appmodel.h>
    #include <xaudio2.h>
    #pragma comment(lib, "xaudio2.lib")
#endif

#include <atomic>
#include <cstdio>
#include <thread>
#include <vector>

namespace gv {

bool IsXboxApp() {
#if defined(_WIN32)
    static const bool packaged = [] {
        UINT32 len = 0;
        return GetCurrentPackageFullName(&len, nullptr) == ERROR_INSUFFICIENT_BUFFER;
    }();
    return packaged;
#else
    return false;
#endif
}

static std::string LogPath() {
    char *pref = SDL_GetPrefPath("", "Ymir");
    std::string path = pref ? pref : "";
    SDL_free(pref);
    return path + "xbox_start.txt";
}

void XboxLogReset() {
    if (!IsXboxApp()) {
        return;
    }
    if (FILE *f = fopen(LogPath().c_str(), "w")) {
        fclose(f);
    }
}

void XboxLog(const std::string &line) {
    if (!IsXboxApp()) {
        return;
    }
    if (FILE *f = fopen(LogPath().c_str(), "a")) {
        fprintf(f, "%s\n", line.c_str());
        fclose(f);
    }
}

#if defined(_WIN32)
struct XboxAudio::Impl {
    IXAudio2 *xa = nullptr;
    IXAudio2MasteringVoice *master = nullptr;
    IXAudio2SourceVoice *voice = nullptr;
    std::thread feeder;
    std::atomic_bool quit = false;
    std::function<void(short *, int)> pull;
    static constexpr int kBuffers = 4;
    int frames = 0;
    std::vector<short> bufs[kBuffers];
};

bool XboxAudio::Start(int sampleRate, std::function<void(short *, int)> pull) {
    Stop();
    auto *m = new Impl();
    m->pull = std::move(pull);
    if (FAILED(XAudio2Create(&m->xa, 0, XAUDIO2_DEFAULT_PROCESSOR)) ||
        FAILED(m->xa->CreateMasteringVoice(&m->master))) {
        XboxLog("sound: XAudio2 failed to start");
        if (m->xa) {
            m->xa->Release();
        }
        delete m;
        return false;
    }
    WAVEFORMATEX wf{};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = sampleRate;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 4;
    wf.nAvgBytesPerSec = sampleRate * 4;
    if (FAILED(m->xa->CreateSourceVoice(&m->voice, &wf))) {
        XboxLog("sound: no XAudio2 voice");
        m->master->DestroyVoice();
        m->xa->Release();
        delete m;
        return false;
    }
    m->frames = sampleRate / 200; // 5 ms per buffer
    for (auto &b : m->bufs) {
        b.assign(m->frames * 2, 0);
    }
    m->voice->Start();
    m_impl = m;
    m->feeder = std::thread([m] {
        int next = 0;
        while (!m->quit) {
            XAUDIO2_VOICE_STATE st{};
            m->voice->GetState(&st, XAUDIO2_VOICE_NOSAMPLESPLAYED);
            if (st.BuffersQueued < Impl::kBuffers - 1) {
                auto &b = m->bufs[next];
                m->pull(b.data(), m->frames);
                XAUDIO2_BUFFER xb{};
                xb.AudioBytes = (UINT32)(b.size() * sizeof(short));
                xb.pAudioData = (const BYTE *)b.data();
                m->voice->SubmitSourceBuffer(&xb);
                next = (next + 1) % Impl::kBuffers;
            } else {
                Sleep(1);
            }
        }
    });
    XboxLog("sound: XAudio2 " + std::to_string(sampleRate) + " Hz");
    return true;
}

void XboxAudio::Stop() {
    if (!m_impl) {
        return;
    }
    m_impl->quit = true;
    if (m_impl->feeder.joinable()) {
        m_impl->feeder.join();
    }
    m_impl->voice->Stop();
    m_impl->voice->DestroyVoice();
    m_impl->master->DestroyVoice();
    m_impl->xa->Release();
    delete m_impl;
    m_impl = nullptr;
}

void XboxAudio::SetVolume(float volume) {
    if (m_impl) {
        m_impl->voice->SetVolume(volume);
    }
}
#else
bool XboxAudio::Start(int, std::function<void(short *, int)>) {
    return false;
}
void XboxAudio::Stop() {}
void XboxAudio::SetVolume(float) {}
#endif

} // namespace gv
