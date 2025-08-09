#pragma once
#include <vector>
#include <string>
#include "roi.h"
namespace mvp_detector {
bool is_mvp_screen(const std::vector<unsigned char>& bgra, int w, int h, const Rect& roi, bool use_ocr = false);
double mvp_confidence(const std::vector<unsigned char>& bgra, int w, int h, const Rect& roi, bool use_ocr = false);
void set_confidence_threshold(double t);
std::string last_mvp_text();
}