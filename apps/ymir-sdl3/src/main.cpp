#include "app/app.hpp"

#include <util/os_exception_handler.hpp>
#include <ymir/util/thread_name.hpp>

#include <cxxopts.hpp>
#include <fmt/format.h>

#if defined(__APPLE__)
    #include <objc/message.h>
#endif

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

static int SDLCALL GvXboxMain(int, char **) {
    // SDL audio devices hang in an Xbox app: Ymir's sound goes through XAudio2 (gv::XboxAudio)
    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy");
    // Direct3D 12 renderer: swap chain in real pixels on the Xbox (our sdl3-uwp)
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d12");
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
    gv::XboxLog("profile: " + s_profile);
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
