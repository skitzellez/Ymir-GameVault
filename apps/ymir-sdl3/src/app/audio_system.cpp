#include "audio_system.hpp"

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_log.h>

#include <algorithm>
#include <string>

#include <gv/gv_xbox.hpp> // Game Vault (Xbox): XAudio2 sound

namespace app {

bool AudioSystem::Init(int sampleRate, SDL_AudioFormat format, int channels, uint32 bufferSize) {
    {
        std::string bufferSizeStr = std::to_string(bufferSize);
        SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, bufferSizeStr.c_str());
    }

    // Game Vault (Xbox): SDL audio devices hang inside an Xbox app; Start() plays through XAudio2 instead
    if (gv::IsXboxApp()) {
        m_gvRate = sampleRate;
        m_gvChannels = channels;
        m_gvFormat = format;
        return true;
    }

    SDL_AudioSpec audioSpec{};
    audioSpec.freq = sampleRate;
    audioSpec.format = format;
    audioSpec.channels = channels;
    m_audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec,
        [](void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
            static_cast<AudioSystem *>(userdata)->ProcessAudioCallback(stream, additional_amount, total_amount);
        },
        this);
    if (m_audioStream == nullptr) {
        return false;
    }
    UpdateGain();
    return true;
}

void AudioSystem::Deinit() {
    m_bufferNotFullEvent.Set();
    m_gvAudio.Stop();
    if (m_audioStream != nullptr) {
        SDL_DestroyAudioStream(m_audioStream);
    }
}

bool AudioSystem::Start() {
    if (gv::IsXboxApp()) {
        m_running = m_gvAudio.Start(m_gvRate, [this](short *frames, int count) { GvPull(frames, count); });
        UpdateGain();
        return m_running;
    }
    if (SDL_ResumeAudioStreamDevice(m_audioStream)) {
        m_running = true;
        return true;
    } else {
        return false;
    }
}

bool AudioSystem::Stop() {
    if (gv::IsXboxApp()) {
        m_gvAudio.Stop();
        m_running = false;
        m_bufferNotFullEvent.Set();
        return true;
    }
    if (SDL_PauseAudioStreamDevice(m_audioStream)) {
        m_running = false;
        m_bufferNotFullEvent.Set();
        return true;
    } else {
        return false;
    }
}

bool AudioSystem::IsRunning() const {
    return m_running;
}

bool AudioSystem::GetAudioStreamFormat(int *sampleRate, SDL_AudioFormat *format, int *channels) {
    if (gv::IsXboxApp()) {
        *sampleRate = m_gvRate;
        *format = m_gvFormat;
        *channels = m_gvChannels;
        return true;
    }
    SDL_AudioSpec srcSpec{};
    SDL_AudioSpec dstSpec{};
    if (SDL_GetAudioStreamFormat(m_audioStream, &srcSpec, &dstSpec)) {
        *sampleRate = srcSpec.freq;
        *format = srcSpec.format;
        *channels = srcSpec.channels;
        return true;
    } else {
        return false;
    }
}

void AudioSystem::ReceiveSample(sint16 left, sint16 right) {
    // If we're doing audio sync, wait until the buffer is no longer full.
    // Otherwise, simply overrun the buffer.
    while (m_sync && !m_silent) {
        const uint32 readPos = m_readPos.load(std::memory_order_acquire);
        const uint32 writePos = m_writePos.load(std::memory_order_relaxed);
        const uint32 count = (writePos >= readPos) ? (writePos - readPos) : (m_buffer.size() - (readPos - writePos));
        if (count < m_buffer.size() - 1) {
            break;
        }

        m_bufferNotFullEvent.Reset();

        const uint32 recheckReadPos = m_readPos.load(std::memory_order_acquire);
        const uint32 recheckCount = (writePos >= recheckReadPos) ? (writePos - recheckReadPos)
                                                                 : (m_buffer.size() - (recheckReadPos - writePos));
        if (recheckCount < m_buffer.size() - 1) {
            m_bufferNotFullEvent.Set();
            break;
        }

        m_bufferNotFullEvent.Wait();
    }

    const uint32 writePos = m_writePos.load(std::memory_order_relaxed);
    m_buffer[writePos] = {left, right};
    m_writePos.store((writePos + 1) % m_buffer.size(), std::memory_order_release);
}

void AudioSystem::UpdateGain() {
    if (gv::IsXboxApp()) {
        m_gvAudio.SetVolume(m_mute ? 0.0f : m_gain);
        return;
    }
    SDL_SetAudioStreamGain(m_audioStream, m_mute ? 0.0f : m_gain);
}

void AudioSystem::ProcessAudioCallback(SDL_AudioStream *stream, int additional_amount, int total_amount) {
    const int sampleCount = additional_amount / sizeof(Sample);
    if (m_silent) {
        const sint16 zero = 0;
        for (int i = 0; i < sampleCount; i++) {
            SDL_PutAudioStreamData(stream, &zero, sizeof(zero));
        }
    } else {
        // Copy the requested amount of samples into the buffer, even if there aren't that many samples available.
        // This effectively implements a crude form of audio stretching that sounds acceptable when the emulator is not
        // running at 100% speed.
        const uint32 readPos = m_readPos.load(std::memory_order_acquire);
        const int len1 = std::min<int>(sampleCount, m_buffer.size() - readPos);
        const int len2 = std::min<int>(sampleCount - len1, readPos);
        SDL_PutAudioStreamData(stream, &m_buffer[readPos], len1 * sizeof(Sample));
        SDL_PutAudioStreamData(stream, &m_buffer[0], len2 * sizeof(Sample));

        m_readPos.store((readPos + len1 + len2) % m_buffer.size(), std::memory_order_release);
        m_bufferNotFullEvent.Set();
    }
}

// Game Vault (Xbox): same as ProcessAudioCallback, for the XAudio2 feeder
void AudioSystem::GvPull(short *frames, int count) {
    Sample *out = reinterpret_cast<Sample *>(frames);
    if (m_silent) {
        std::fill(out, out + count, Sample{0, 0});
        return;
    }
    const uint32 readPos = m_readPos.load(std::memory_order_acquire);
    const int len1 = std::min<int>(count, m_buffer.size() - readPos);
    const int len2 = std::min<int>(count - len1, readPos);
    std::copy_n(&m_buffer[readPos], len1, out);
    std::copy_n(&m_buffer[0], len2, out + len1);
    m_readPos.store((readPos + len1 + len2) % m_buffer.size(), std::memory_order_release);
    m_bufferNotFullEvent.Set();
}

} // namespace app
