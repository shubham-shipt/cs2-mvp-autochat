#pragma once
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
class HotkeyManager {
public:
    using Callback = std::function<void()>;
    HotkeyManager();
    ~HotkeyManager();
    bool register_hotkey(Display* dpy, Window root, KeySym sym, unsigned int modifiers = 0, Callback cb = nullptr);
    void unregister_all(Display* dpy, Window root);
    void start(Display* dpy, Window root);
    void stop();
    static bool wait_for_hotkey(Display* dpy, KeySym sym, int poll_ms = 50);
private:
    std::atomic<bool> running_;
    std::thread thread_;
    std::vector<std::tuple<KeySym, unsigned int, Callback>> hotkeys_;
};