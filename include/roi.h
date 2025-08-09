#pragma once
#include <string>
#include <sstream>
#include <algorithm>
struct Rect {
    int x{0}, y{0}, w{0}, h{0};
    Rect() = default;
    Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
    bool empty() const { return w <= 0 || h <= 0; }
    void clamp(int max_w, int max_h) {
        x = std::clamp(x, 0, max_w);
        y = std::clamp(y, 0, max_h);
        w = std::clamp(w, 0, max_w - x);
        h = std::clamp(h, 0, max_h - y);
    }
    std::string str() const {
        std::ostringstream oss;
        oss << "Rect(" << x << "," << y << "," << w << "," << h << ")";
        return oss.str();
    }
};