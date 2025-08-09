#pragma once
#include <string>
#include <vector>
#include <X11/Xlib.h>
#include <X11/keysym.h>
namespace keyboard {
bool key_event(Display* dpy, KeySym sym, bool press);
bool type_text(Display* dpy, const std::string& text, int delay_ms = 10);
bool type_keys(Display* dpy, const std::vector<KeySym>& syms, int delay_ms = 20);
bool press_enter(Display* dpy);
bool map_char(char c, KeySym& sym, bool& need_shift);
}