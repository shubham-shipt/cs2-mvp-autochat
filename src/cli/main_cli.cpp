#include <X11/Xlib.h>
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

#include "version.h"
#include "logger.h"
#include "config.h"
#include "roi.h"
#include "window_finder.h"
#include "capture.h"
#include "mvp_detector.h"
#include "state_machine.h"
#include "keyboard.h"

static volatile bool running = true;
void handle_sigint(int) { running = false; }

int main(int argc, char** argv) {
    std::cout << appinfo::name() << " v" << appinfo::version() << std::endl;
    logger::default_logger().info("Starting CLI...");

    AppConfig cfg;
    if (argc > 1) {
        if (!cfg.load(argv[1])) {
            std::cerr << "Failed to load config: " << argv[1] << std::endl;
            return 1;
        }
    } else {
        cfg.load("config.json");
    }
    logger::default_logger().info("Config loaded.");

    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        std::cerr << "Failed to open X11 display." << std::endl;
        return 1;
    }

    Window win = window_finder::find_window_by_title(dpy, cfg.window_title);
    if (!win) {
        std::cerr << "Could not find window with title: " << cfg.window_title << std::endl;
        return 1;
    }
    logger::default_logger().info("Target window found.");

    int wx, wy, ww, wh;
    if (!window_finder::get_window_geometry(dpy, win, wx, wy, ww, wh)) {
        std::cerr << "Failed to get window geometry." << std::endl;
        return 1;
    }
    Rect roi = cfg.use_mvp ? cfg.mvp_roi : Rect(0, 0, ww, wh);

    state_machine::StateMachine sm;
    sm.set_debounce_ms(cfg.debounce_ms);

    std::signal(SIGINT, handle_sigint);

    std::vector<unsigned char> frame;
    int fw = 0, fh = 0;

    logger::default_logger().info("Entering main loop. Press Ctrl+C to exit.");

    while (running) {
        if (!capture::capture_window_bgra(dpy, win, roi, frame, fw, fh) || frame.empty()) {
            logger::default_logger().warn("Failed to capture window.");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        bool is_mvp = mvp_detector::is_mvp_screen(frame, fw, fh, roi, cfg.enable_ocr);
        state_machine::GameState gs = is_mvp ? state_machine::GameState::MVPShown : state_machine::GameState::InMatch;
        sm.update(gs, std::chrono::steady_clock::now());
        if (sm.should_send_message()) {
            logger::default_logger().info("Detected MVP screen. Sending message...");
            window_finder::focus_window(dpy, win);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            keyboard::key_event(dpy, cfg.chat_key == 'u' ? XK_u : XK_y, true);
            keyboard::key_event(dpy, cfg.chat_key == 'u' ? XK_u : XK_y, false);
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg.delay_ms));
            keyboard::type_text(dpy, cfg.message, cfg.delay_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg.delay_ms));
            keyboard::press_enter(dpy);
            logger::default_logger().info("Message sent.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    XCloseDisplay(dpy);
    logger::default_logger().info("Exited cleanly.");
    return 0;
}