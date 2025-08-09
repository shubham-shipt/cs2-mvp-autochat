#pragma once
#include <string>
#include <vector>
namespace ocr {
std::string extract_text(const std::vector<unsigned char>& bgra, int w, int h, const std::string& lang = "eng");
void set_ocr_bin(const std::string& path);
void set_ocr_lang(const std::string& lang);
}