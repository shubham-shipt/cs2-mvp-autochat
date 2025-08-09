#include "ocr.h"
#include <string>
#include <vector>

namespace ocr {

static std::string ocr_bin_path = "tesseract";
static std::string ocr_lang_code = "eng";

std::string extract_text(const std::vector<unsigned char>& bgra, int w, int h, const std::string& lang) {
    return ""; // No OCR backend in stub
}

void set_ocr_bin(const std::string& path) {
    ocr_bin_path = path;
}

void set_ocr_lang(const std::string& lang) {
    ocr_lang_code = lang;
}

}