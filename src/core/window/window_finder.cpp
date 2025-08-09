#include "window_finder.h"
#include <X11/Xatom.h>
#include <algorithm>
#include <cctype>

namespace window_finder {

static std::string tolower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return std::tolower(c); });
    return r;
}

std::vector<Window> list_windows(Display* dpy) {
    std::vector<Window> wins;
    Window root = DefaultRootWindow(dpy);
    Window parent;
    Window *children;
    unsigned int nchildren;
    if (XQueryTree(dpy, root, &root, &parent, &children, &nchildren)) {
        for (unsigned int i = 0; i < nchildren; ++i)
            wins.push_back(children[i]);
        if (children) XFree(children);
    }
    return wins;
}

std::string get_window_title(Display* dpy, Window win) {
    Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    Atom prop = XInternAtom(dpy, "_NET_WM_NAME", False);
    Atom actual;
    int format;
    unsigned long n, left;
    unsigned char* data = nullptr;
    if (XGetWindowProperty(dpy, win, prop, 0, (~0L), False, utf8, &actual, &format, &n, &left, &data) == Success && data) {
        std::string title((char*)data, n);
        XFree(data);
        return title;
    }
    prop = XInternAtom(dpy, "WM_NAME", False);
    if (XGetWindowProperty(dpy, win, prop, 0, (~0L), False, utf8, &actual, &format, &n, &left, &data) == Success && data) {
        std::string title((char*)data, n);
        XFree(data);
        return title;
    }
    return "";
}

Window find_window_by_title(Display* dpy, const std::string& needle) {
    auto wins = list_windows(dpy);
    std::string n = tolower(needle);
    for (Window w : wins) {
        std::string t = tolower(get_window_title(dpy, w));
        if (t.find(n) != std::string::npos) return w;
    }
    return 0;
}

bool focus_window(Display* dpy, Window win) {
    XRaiseWindow(dpy, win);
    XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
    XFlush(dpy);
    return true;
}

bool get_window_geometry(Display* dpy, Window win, int& x, int& y, int& w, int& h) {
    XWindowAttributes attrs;
    if (!XGetWindowAttributes(dpy, win, &attrs)) return false;
    x = attrs.x; y = attrs.y; w = attrs.width; h = attrs.height;
    return true;
}

}