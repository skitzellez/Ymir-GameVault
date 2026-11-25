#include "app/app.hpp"

#include <util/os_exception_handler.hpp>
#include <ymir/util/thread_name.hpp>

#include <cxxopts.hpp>
#include <fmt/format.h>

#if defined(__APPLE__)
    #include <objc/message.h>
#endif

#include <exception>
#include <memory>

#include <gv/gv_xbox.hpp>

#if defined(_WIN32)
// Copyright (c) 2026 Skitzelles - part of Game Vault, https://github.com/skitzellez/xbox-game-vault
// Game Vault (Xbox): start-up as an Xbox app. Game Vault starts a disc with the link
// gamevault-ymir:?disc=<path>[&launchOnExit=gamevault:...] (cmd= works too); started from the tile, Ymir opens without a
// disc. Settings, BIOS (roms\ipl) and saves live in LocalState\Ymir.
    #include <SDL3/SDL_hints.h>
    #include <SDL3/SDL_init.h>
    #include <SDL3/SDL_events.h>
    #include <SDL3/SDL_filesystem.h>
    #include <SDL3/SDL_timer.h>
    #define SDL_MAIN_NOIMPL
    #include <SDL3/SDL_main.h>
    #undef main

    #include <Windows.h>
    #include <cctype>
    #include <cstdio>
    #include <filesystem>
    #include <vector>
    #include <cstdlib>
    #include <string>

static std::string GvUrlDecode(const std::string &in) {
    std::string out;
    for (size_t i = 0; i < in.size(); i++) {
        if (in[i] == '%' && i + 2 < in.size() && isxdigit((unsigned char)in[i + 1]) &&
            isxdigit((unsigned char)in[i + 2])) {
            out += (char)strtol(in.substr(i + 1, 2).c_str(), nullptr, 16);
            i += 2;
        } else {
            out += in[i] == '+' ? ' ' : in[i];
        }
    }
    return out;
}

static std::string GvLinkValue(const std::string &link, const char *key) {
    const std::string k = std::string(key) + "=";
    size_t q = link.find("?" + k);
    if (q == std::string::npos) {
        q = link.find("&" + k);
    }
    if (q == std::string::npos) {
        return {};
    }
    q += 1 + k.size();
    const size_t end = link.find('&', q);
    return GvUrlDecode(link.substr(q, end == std::string::npos ? std::string::npos : end - q));
}

int main(int argc, char **argv);

// Game Vault (Xbox): a Saturn BIOS (the player's own) is looked for when roms\ipl has none: the disc's folder, the folder
// above it, then the usual BIOS folders on every drive; the first one found is copied into roms\ipl.
static void GvFindSaturnBios(const std::string &profile, const std::string &disc) {
    const std::wstring ipl = std::filesystem::path(std::u8string(profile.begin(), profile.end())).wstring() + L"roms\\ipl";
    std::error_code ec;
    std::filesystem::create_directories(ipl, ec);
    for (const auto &e : std::filesystem::directory_iterator(ipl, ec)) {
        if (e.is_regular_file(ec)) {
            gv::XboxLog("bios: already in roms\\ipl");
            return;
        }
    }
    static const wchar_t *kNames[] = {L"sega_101.bin", L"mpr-17933.bin", L"sega1003.bin", L"sega_100.bin",
                                      L"mpr-18100.bin", L"saturn_bios.bin", L"Sega Saturn BIOS (USA).bin"};
    std::vector<std::filesystem::path> dirs;
    if (!disc.empty()) {
        const std::filesystem::path d = std::filesystem::path(std::u8string(disc.begin(), disc.end())).parent_path();
        dirs.push_back(d);
        dirs.push_back(d.parent_path());
    }
    static const wchar_t *kSub[] = {L"Game Vault\\bios", L"bios", L"BIOS", L"system", L"RetroArch\\system",
                                    L"RetroArch\\bios", L"roms\\bios", L"Emulation\\bios", L"Saturn", L"TEST GAMES", L""};
    for (wchar_t drive = L'D'; drive <= L'Z'; drive++) {
        for (const wchar_t *sub : kSub) {
            dirs.push_back(std::filesystem::path(std::wstring(1, drive) + L":\\" + sub));
        }
    }
    for (const auto &dir : dirs) {
        for (const wchar_t *name : kNames) {
            const auto src = dir / name;
            if (std::filesystem::is_regular_file(src, ec) && std::filesystem::file_size(src, ec) == 512 * 1024) {
                const auto dst = std::filesystem::path(ipl) / name;
                if (std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing, ec)) {
                    gv::XboxLog("bios: copied " + src.string() + " into roms\\ipl");
                } else {
                    gv::XboxLog("bios: found " + src.string() + " but the copy failed: " + ec.message());
                }
                return;
            }
        }
    }
    gv::XboxLog("bios: no Saturn BIOS found (looked for sega_101.bin, mpr-17933.bin and others)");
}

static int SDLCALL GvXboxMain(int, char **) {
    // SDL audio devices hang in an Xbox app: Ymir's sound goes through XAudio2 (gv::XboxAudio)
    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    // Direct3D 12 renderer: swap chain in real pixels on the Xbox (our sdl3-uwp)
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d12");
    // Windows.Gaming.Input: the only controller API in an Xbox app (our SDL keeps it off unless asked)
    SDL_SetHint("SDL_JOYSTICK_WGI", "1");
    // The activation (and its link) arrives while SDL pumps the window's events
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    const char *uri = nullptr;
    for (int i = 0; i < 300 && (uri = SDL_GetHint("SDL_WINRT_ACTIVATION_URI")) == nullptr; i++) {
        SDL_PumpEvents();
        SDL_Delay(10);
    }
    const std::string link = uri ? uri : "";

    char *pref = SDL_GetPrefPath("", "Ymir");
    static std::string s_profile;
    s_profile = pref ? pref : "";
    SDL_free(pref);
    gv::XboxLogReset();
    gv::XboxLog("Ymir for Game Vault " Ymir_VERSION ", link: " + link);
    {
        char base[32];
        snprintf(base, sizeof(base), "%p", (void *)GetModuleHandleW(nullptr));
        gv::XboxLog(std::string("exe base: ") + base);
    }

    static std::string s_disc;
    s_disc = GvLinkValue(link, "disc");
    if (s_disc.empty()) {
        s_disc = GvLinkValue(link, "cmd");
    }
    if (s_disc.empty()) {
        // test hook: LocalState\Ymir\gvtest_disc.txt holds a disc path for a start from the tile
        if (FILE *f = fopen((s_profile + "gvtest_disc.txt").c_str(), "r")) {
            char line[1024] = {};
            if (fgets(line, sizeof(line), f)) {
                s_disc = line;
                while (!s_disc.empty() && (s_disc.back() == 0x0A || s_disc.back() == 0x0D)) {
                    s_disc.pop_back();
                }
            }
            fclose(f);
        }
    }
    gv::XboxLog("profile: " + s_profile);
    GvFindSaturnBios(s_profile, s_disc);
    gv::XboxLog("disc: " + (s_disc.empty() ? std::string("(none)") : s_disc));

    static const char *s_argv[8];
    int argc = 0;
    s_argv[argc++] = "Ymir";
    s_argv[argc++] = "-p";
    s_argv[argc++] = s_profile.c_str();
    s_argv[argc++] = "-f";
    if (!s_disc.empty()) {
        s_argv[argc++] = s_disc.c_str();
    }
    s_argv[argc] = nullptr;
    const int ret = main(argc, const_cast<char **>(s_argv));
    gv::XboxLog("exit code " + std::to_string(ret));
    return ret;
}
#endif

int main(int argc, char **argv) {
#if defined(_WIN32)
    // Game Vault (Xbox): inside an app package this is an Xbox app. Our sdl3-uwp runs the WinRT app and calls
    // GvXboxMain (which calls main again with the real options) from the app thread.
    static bool s_gvStarted = false;
    if (gv::IsXboxApp() && !s_gvStarted) {
        s_gvStarted = true;
        return SDL_RunApp(0, nullptr, GvXboxMain, nullptr);
    }
    // NOTE: Setting the main thread name on Linux and macOS replaces the process name displayed on tools like `top`.
    util::SetCurrentThreadName("Main thread");
#endif

    bool showHelp = false;
    bool enableAllExceptions = false;

    app::CommandLineOptions progOpts{};
    cxxopts::Options options("Ymir", "Ymir - Sega Saturn emulator");
    options.add_options()("d,disc", "Path to Saturn disc image (.ccd, .chd, .cue, .iso, .mds)",
                          cxxopts::value(progOpts.gameDiscPath));
    options.add_options()("p,profile", "Path to profile directory", cxxopts::value(progOpts.profilePath));
    options.add_options()("u,user", "Force user profile",
                          cxxopts::value(progOpts.forceUserProfile)->default_value("false"));
    options.add_options()("h,help", "Display help text", cxxopts::value(showHelp)->default_value("false"));
    options.add_options()("f,fullscreen", "Start in fullscreen mode",
                          cxxopts::value(progOpts.fullScreen)->default_value("false"));
    options.add_options()("P,paused", "Start paused", cxxopts::value(progOpts.startPaused)->default_value("false"));
    options.add_options()("F,fast-forward", "Start in fast-forward mode",
                          cxxopts::value(progOpts.startFastForward)->default_value("false"));
    options.add_options()("D,debug", "Start with debug tracing enabled",
                          cxxopts::value(progOpts.enableDebugTracing)->default_value("false"));
    options.add_options()("E,exceptions", "Capture all unhandled exceptions",
                          cxxopts::value(enableAllExceptions)->default_value("false"));
    options.parse_positional({"disc"});
    options.positional_help("path to disc image");
    options.show_positional_help();

#if !Ymir_LOCAL_BUILD
    try {
#endif
        auto result = options.parse(argc, argv);
        if (showHelp) {
            fmt::println("{}", options.help());
            return 0;
        }

        gv::XboxLog("main: options read");
        // Game Vault (Xbox): an unhandled C++ error on any thread ends the app silently; log it first
        std::set_terminate([] {
            std::string what = "unknown";
            if (auto e = std::current_exception()) {
                try {
                    std::rethrow_exception(e);
                } catch (const std::exception &ex) {
                    what = ex.what();
                } catch (...) {
                }
            }
            gv::XboxLog("terminate: unhandled error: " + what);
            std::abort();
        });
        util::RegisterExceptionHandler(enableAllExceptions);
        gv::XboxLog("main: exception handler on");

        auto app = std::make_unique<app::App>();
        return app->Run(progOpts);
#if !Ymir_LOCAL_BUILD
    } catch (const cxxopts::exceptions::exception &e) {
        std::string msg = fmt::format("Failed to parse arguments: {}", e.what());
        fmt::println("{}", msg);
        util::ShowFatalErrorDialog(msg.c_str());
        return -1;
    } catch (const std::system_error &e) {
        std::string msg = fmt::format("System error: {}", e.what());
        fmt::println("{}", msg);
        util::ShowFatalErrorDialog(msg.c_str());
        return e.code().value();
    } catch (const std::exception &e) {
        std::string msg = fmt::format("Unhandled exception: {}", e.what());
        fmt::println("{}", msg);
        util::ShowFatalErrorDialog(msg.c_str());
        return -1;
    #if defined(__APPLE__)
    } catch (id e) {
        SEL sel_reason = sel_registerName("reason");
        id reason = ((id (*)(id, SEL))objc_msgSend)(e, sel_reason); // NSString

        SEL sel_UTF8String = sel_registerName("UTF8String");
        const char *failureReason = ((const char *(*)(id, SEL))objc_msgSend)(reason, sel_UTF8String);
        util::ShowFatalErrorDialog(failureReason);
        return -1;
    #endif
    } catch (...) {
        std::string msg = "Unspecified exception";
        fmt::println("{}", msg);
        util::ShowFatalErrorDialog(msg.c_str());
        return -1;
    }
#endif

    return 0;
}
