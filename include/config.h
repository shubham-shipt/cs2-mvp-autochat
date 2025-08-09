#pragma once
#include <string>
#include <vector>
#include <map>
#include "roi.h"
struct AppConfig {
    std::string message = "gl hf";
    int delay_ms = 20;
    char chat_key = 'y';
    std::string window_title = "Counter-Strike 2";
    bool use_mvp = false;
    Rect mvp_roi = Rect(0,0,0,0);
    std::string theme_css = "assets/themes/ui.css";
    std::string log_file = "logs/app.log";
    bool debug_mode = false;
    std::map<std::string,std::string> profiles;
    int max_log_files = 3;
    size_t max_log_size = 1 * 1024 * 1024;
    bool enable_ocr = false;
    std::string ocr_lang = "eng";
    std::string ocr_bin = "tesseract";
    int debounce_ms = 500;
    bool load(const std::string& path);
    bool save(const std::string& path) const;
    void apply_profile(const std::string& profile_name);
};
bool load_config_file(const std::string& path, AppConfig& cfg);
bool save_config_file(const std::string& path, const AppConfig& cfg);