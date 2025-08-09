#include "mvp_detector.h"
#include "ocr.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace mvp_detector {

static double conf_threshold = 0.6;
static std::string last_text;

bool is_mvp_screen(const std::vector<unsigned char>& bgra, int w, int h, const Rect& roi, bool use_ocr) {
    double conf = mvp_confidence(bgra, w, h, roi, use_ocr);
    return conf >= conf_threshold;
}

double mvp_confidence(const std::vector<unsigned char>& bgra, int w, int h, const Rect& roi, bool use_ocr) {
    if (w == 0 || h == 0 || bgra.empty()) return 0.0;
    int rx = roi.x, ry = roi.y, rw = roi.w, rh = roi.h;
    if (rw <= 0 || rh <= 0) { rx = 0; ry = 0; rw = w; rh = h; }
    int n = 0, yellowish = 0;
    for (int y = ry; y < ry + rh && y < h; ++y) {
        for (int x = rx; x < rx + rw && x < w; ++x) {
            const unsigned char* p = &bgra[4 * (y * w + x)];
            int b = p[0], g = p[1], r = p[2];
            if (r > 200 && g > 190 && b < 100 && std::abs(r - g) < 30) yellowish++;
            n++;
        }
    }
    double heur = n > 0 ? double(yellowish) / n : 0.0;
    double ocr_score = 0.0;
    if (use_ocr) {
        std::string text = ocr::extract_text(bgra, w, h, "eng");
        last_text = text;
        if (text.find("MVP") != std::string::npos || text.find("Most Valuable") != std::string::npos)
            ocr_score = 1.0;
    }
    return std::max(heur, ocr_score);
}

void set_confidence_threshold(double t) {
    conf_threshold = t;
}

std::string last_mvp_text() {
    return last_text;
}

}