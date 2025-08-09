#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool AppConfig::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    std::string ext = path.substr(path.find_last_of('.') + 1);
    if (ext == "json") {
        json j;
        in >> j;
        message = j.value("message", message);
        delay_ms = j.value("delay_ms", delay_ms);
        chat_key = j.value("chat_key", std::string(1, chat_key))[0];
        window_title = j.value("window_title", window_title);
        use_mvp = j.value("use_mvp", use_mvp);
        debug_mode = j.value("debug_mode", debug_mode);
        max_log_files = j.value("max_log_files", max_log_files);
        max_log_size = j.value("max_log_size", max_log_size);
        enable_ocr = j.value("enable_ocr", enable_ocr);
        ocr_lang = j.value("ocr_lang", ocr_lang);
        ocr_bin = j.value("ocr_bin", ocr_bin);
        debounce_ms = j.value("debounce_ms", debounce_ms);
        theme_css = j.value("theme_css", theme_css);
        log_file = j.value("log_file", log_file);
        if (j.contains("mvp_roi")) {
            auto r = j["mvp_roi"];
            mvp_roi = Rect(r.value("x", 0), r.value("y", 0), r.value("w", 0), r.value("h", 0));
        }
        if (j.contains("profiles")) {
            for (auto& [k, v] : j["profiles"].items()) profiles[k] = v;
        }
        return true;
    } else if (ext == "ini" || ext == "conf" || ext == "cfg") {
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);
            if (key == "message") message = val;
            else if (key == "delay_ms") delay_ms = std::stoi(val);
            else if (key == "chat_key") chat_key = val[0];
            else if (key == "window_title") window_title = val;
            else if (key == "use_mvp") use_mvp = std::stoi(val) != 0;
            else if (key == "debug_mode") debug_mode = std::stoi(val) != 0;
            else if (key == "max_log_files") max_log_files = std::stoi(val);
            else if (key == "max_log_size") max_log_size = std::stoul(val);
            else if (key == "enable_ocr") enable_ocr = std::stoi(val) != 0;
            else if (key == "ocr_lang") ocr_lang = val;
            else if (key == "ocr_bin") ocr_bin = val;
            else if (key == "debounce_ms") debounce_ms = std::stoi(val);
            else if (key == "theme_css") theme_css = val;
            else if (key == "log_file") log_file = val;
            else if (key == "mvp_roi") {
                int x, y, w, h;
                std::sscanf(val.c_str(), "%d,%d,%d,%d", &x, &y, &w, &h);
                mvp_roi = Rect(x, y, w, h);
            }
        }
        return true;
    }
    return false;
}

bool AppConfig::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    std::string ext = path.substr(path.find_last_of('.') + 1);
    if (ext == "json") {
        json j;
        j["message"] = message;
        j["delay_ms"] = delay_ms;
        j["chat_key"] = std::string(1, chat_key);
        j["window_title"] = window_title;
        j["use_mvp"] = use_mvp;
        j["debug_mode"] = debug_mode;
        j["max_log_files"] = max_log_files;
        j["max_log_size"] = max_log_size;
        j["enable_ocr"] = enable_ocr;
        j["ocr_lang"] = ocr_lang;
        j["ocr_bin"] = ocr_bin;
        j["debounce_ms"] = debounce_ms;
        j["theme_css"] = theme_css;
        j["log_file"] = log_file;
        j["mvp_roi"] = { {"x", mvp_roi.x}, {"y", mvp_roi.y}, {"w", mvp_roi.w}, {"h", mvp_roi.h} };
        j["profiles"] = profiles;
        out << j.dump(4) << std::endl;
        return true;
    } else if (ext == "ini" || ext == "conf" || ext == "cfg") {
        out << "message=" << message << "\n";
        out << "delay_ms=" << delay_ms << "\n";
        out << "chat_key=" << chat_key << "\n";
        out << "window_title=" << window_title << "\n";
        out << "use_mvp=" << (use_mvp ? "1" : "0") << "\n";
        out << "debug_mode=" << (debug_mode ? "1" : "0") << "\n";
        out << "max_log_files=" << max_log_files << "\n";
        out << "max_log_size=" << max_log_size << "\n";
        out << "enable_ocr=" << (enable_ocr ? "1" : "0") << "\n";
        out << "ocr_lang=" << ocr_lang << "\n";
        out << "ocr_bin=" << ocr_bin << "\n";
        out << "debounce_ms=" << debounce_ms << "\n";
        out << "theme_css=" << theme_css << "\n";
        out << "log_file=" << log_file << "\n";
        out << "mvp_roi=" << mvp_roi.x << "," << mvp_roi.y << "," << mvp_roi.w << "," << mvp_roi.h << "\n";
        for (const auto& [k, v] : profiles)
            out << "profile_" << k << "=" << v << "\n";
        return true;
    }
    return false;
}

void AppConfig::apply_profile(const std::string& profile_name) {
    auto it = profiles.find(profile_name);
    if (it != profiles.end()) load(it->second);
}

bool load_config_file(const std::string& path, AppConfig& cfg) {
    return cfg.load(path);
}

bool save_config_file(const std::string& path, const AppConfig& cfg) {
    return cfg.save(path);
}