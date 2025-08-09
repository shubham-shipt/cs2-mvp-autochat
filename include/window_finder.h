#pragma once
#include <X11/Xlib.h>
#include <string>
#include <vector>
namespace window_finder {
Window find_window_by_title(Display* dpy, const std::string& needle);
std::vector<Window> list_windows(Display* dpy);
std::string get_window_title(Display* dpy, Window win);
bool focus_window(Display* dpy, Window win);
bool get_window_geometry(Display* dpy, Window win, int& x, int& y, int& w, int& h);
}