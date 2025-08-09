#include "capture.h"
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>

namespace capture {

bool is_xshm_available(Display* dpy) {
    int major, minor;
    Bool pixmaps;
    return XShmQueryVersion(dpy, &major, &minor, &pixmaps);
}

bool capture_window_bgra(Display* dpy, Window win, const Rect& roi,
                         std::vector<unsigned char>& out_bgra,
                         int& width, int& height) {
    int wx, wy, ww, wh;
    XWindowAttributes attrs;
    if (!XGetWindowAttributes(dpy, win, &attrs)) return false;
    wx = attrs.x; wy = attrs.y; ww = attrs.width; wh = attrs.height;
    int rx = roi.x, ry = roi.y, rw = roi.w, rh = roi.h;
    if (rw <= 0 || rh <= 0) { rw = ww; rh = wh; }
    width = rw; height = rh;
    XImage* img = nullptr;
    XShmSegmentInfo shminfo;
    bool use_shm = is_xshm_available(dpy);

    if (use_shm) {
        img = XShmCreateImage(dpy, attrs.visual, attrs.depth, ZPixmap, 0, &shminfo, rw, rh);
        shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height, IPC_CREAT|0777);
        shminfo.shmaddr = img->data = (char*)shmat(shminfo.shmid, 0, 0);
        shminfo.readOnly = False;
        XShmAttach(dpy, &shminfo);
        XShmGetImage(dpy, win, img, rx, ry, AllPlanes);
    } else {
        img = XGetImage(dpy, win, rx, ry, rw, rh, AllPlanes, ZPixmap);
    }
    if (!img) return false;
    out_bgra.resize(rw * rh * 4);
    for (int y = 0; y < rh; ++y) {
        for (int x = 0; x < rw; ++x) {
            unsigned long p = XGetPixel(img, x, y);
            unsigned char r = (p & img->red_mask) >> 16;
            unsigned char g = (p & img->green_mask) >> 8;
            unsigned char b = (p & img->blue_mask);
            out_bgra[4*(y*rw + x) + 0] = b;
            out_bgra[4*(y*rw + x) + 1] = g;
            out_bgra[4*(y*rw + x) + 2] = r;
            out_bgra[4*(y*rw + x) + 3] = 255;
        }
    }
    if (use_shm) {
        XShmDetach(dpy, &shminfo);
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
        XDestroyImage(img);
    } else {
        XDestroyImage(img);
    }
    return true;
}

}