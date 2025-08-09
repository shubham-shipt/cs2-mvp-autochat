#include "hotkeys.h"
#include <unistd.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <algorithm>

HotkeyManager::HotkeyManager() : running_(false) {}
HotkeyManager::~HotkeyManager() { stop(); }

bool HotkeyManager::register_hotkey(Display* dpy, Window root, KeySym sym, unsigned int modifiers, Callback cb) {
    KeyCode code = XKeysymToKeycode(dpy, sym);
    if (!code) return false;
    XGrabKey(dpy, code, modifiers, root, True, GrabModeAsync, GrabModeAsync);
    hotkeys_.push_back({sym, modifiers, cb});
    return true;
}

void HotkeyManager::unregister_all(Display* dpy, Window root) {
    for (const auto& hk : hotkeys_) {
        KeySym sym; unsigned int mod; Callback cb;
        std::tie(sym, mod, cb) = hk;
        KeyCode code = XKeysymToKeycode(dpy, sym);
        XUngrabKey(dpy, code, mod, root);
    }
    hotkeys_.clear();
}

void HotkeyManager::start(Display* dpy, Window root) {
    if (running_) return;
    running_ = true;
    thread_ = std::thread([this, dpy, root]() {
        XEvent ev;
        while (running_) {
            while (XPending(dpy)) {
                XNextEvent(dpy, &ev);
                if (ev.type == KeyPress) {
                    KeySym sym = XkbKeycodeToKeysym(dpy, ev.xkey.keycode, 0, 0);
                    unsigned int mod = ev.xkey.state & (ShiftMask|ControlMask|Mod1Mask|Mod4Mask);
                    for (const auto& hk : hotkeys_) {
                        KeySym hsym; unsigned int hmod; Callback cb;
                        std::tie(hsym, hmod, cb) = hk;
                        if (hsym == sym && hmod == mod && cb) cb();
                    }
                }
            }
            usleep(10000);
        }
    });
}

void HotkeyManager::stop() {
    running_ = false;
    if (thread_.joinable()) thread_.join();
}

bool HotkeyManager::wait_for_hotkey(Display* dpy, KeySym sym, int poll_ms) {
    Window root = DefaultRootWindow(dpy);
    KeyCode code = XKeysymToKeycode(dpy, sym);
    XGrabKey(dpy, code, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
    XEvent ev;
    while (true) {
        if (XPending(dpy)) {
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress && ev.xkey.keycode == code) {
                XUngrabKey(dpy, code, AnyModifier, root);
                return true;
            }
        }
        usleep(poll_ms * 1000);
    }
    return false;
}