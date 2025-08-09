#pragma once
#include <vector>
#include <X11/Xlib.h>
#include "roi.h"
namespace capture {
bool capture_window_bgra(Display* dpy, Window win, const Rect& roi,
                         std::vector<unsigned char>& out_bgra,
                         int& width, int& height);
bool is_xshm_available(Display* dpy);
}