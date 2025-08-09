#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Project headers (already present in your repo structure)
#include "config.h"
#include "window_finder.h"
#include "capture.h"
#include "mvp_detector.h"
#include "state_machine.h"
#include "keyboard.h"

class MainWindow {
public:
    MainWindow() : window_(nullptr),
                   start_button_(nullptr),
                   stop_button_(nullptr),
                   status_label_(nullptr),
                   running_(false),
                   config_path_("config.json") {
        // Build UI
        window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window_), "CS2 AutoChat (GTK)");
        gtk_window_set_default_size(GTK_WINDOW(window_), 420, 220);

        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_container_add(GTK_CONTAINER(window_), vbox);

        status_label_ = gtk_label_new("Status: Idle");
        gtk_box_pack_start(GTK_BOX(vbox), status_label_, TRUE, TRUE, 6);

        start_button_ = gtk_button_new_with_label("Start");
        gtk_box_pack_start(GTK_BOX(vbox), start_button_, FALSE, FALSE, 6);

        stop_button_ = gtk_button_new_with_label("Stop");
        gtk_box_pack_start(GTK_BOX(vbox), stop_button_, FALSE, FALSE, 6);

        // Signals
        g_signal_connect(window_, "destroy", G_CALLBACK(&MainWindow::on_destroy_static), this);
        g_signal_connect(start_button_, "clicked", G_CALLBACK(&MainWindow::on_start_clicked_static), this);
        g_signal_connect(stop_button_, "clicked", G_CALLBACK(&MainWindow::on_stop_clicked_static), this);
    }

    ~MainWindow() {
        stop_detection();
    }

    void show() {
        gtk_widget_show_all(window_);
    }

private:
    // Widgets
    GtkWidget* window_;
    GtkWidget* start_button_;
    GtkWidget* stop_button_;
    GtkWidget* status_label_;

    // Worker
    std::thread worker_;
    std::atomic<bool> running_;
    std::string config_path_;

    // Static signal shims
    static void on_destroy_static(GtkWidget*, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        self->stop_detection();
        gtk_main_quit();
    }

    static void on_start_clicked_static(GtkButton*, gpointer user_data) {
        static_cast<MainWindow*>(user_data)->start_detection();
    }

    static void on_stop_clicked_static(GtkButton*, gpointer user_data) {
        static_cast<MainWindow*>(user_data)->stop_detection();
    }

    // UI helpers
    static gboolean update_label_cb(gpointer user_data) {
        // user_data is a heap-allocated pair of (GtkWidget* label, std::string* text)
        auto* payload = static_cast<std::pair<GtkWidget*, std::string*>*>(user_data);
        if (payload && payload->first && payload->second) {
            gtk_label_set_text(GTK_LABEL(payload->first), payload->second->c_str());
        }
        delete payload->second;
        delete payload;
        return G_SOURCE_REMOVE;
    }

    void update_status(const std::string& status) {
        // Post to GTK main loop
        auto* text = new std::string("Status: " + status);
        auto* payload = new std::pair<GtkWidget*, std::string*>(status_label_, text);
        g_idle_add(&MainWindow::update_label_cb, payload);
    }

    // Core logic
    void start_detection() {
        if (running_) return;
        running_ = true;
        update_status("Starting...");

        worker_ = std::thread([this]() {
            AppConfig cfg;
            if (!cfg.load(config_path_)) {
                update_status("Failed to load config.json");
                running_ = false;
                return;
            }

            Display* dpy = XOpenDisplay(nullptr);
            if (!dpy) {
                update_status("Failed to open X11 display");
                running_ = false;
                return;
            }

            Window win = window_finder::find_window_by_title(dpy, cfg.window_title);
            if (!win) {
                update_status("Target window not found: " + cfg.window_title);
                XCloseDisplay(dpy);
                running_ = false;
                return;
            }

            int wx = 0, wy = 0, ww = 0, wh = 0;
            if (!window_finder::get_window_geometry(dpy, win, wx, wy, ww, wh)) {
                update_status("Failed to get window geometry");
                XCloseDisplay(dpy);
                running_ = false;
                return;
            }

            Rect roi = cfg.use_mvp ? cfg.mvp_roi : Rect(0, 0, ww, wh);
            state_machine::StateMachine sm;
            sm.set_debounce_ms(cfg.debounce_ms);

            std::vector<unsigned char> frame;
            int fw = 0, fh = 0;

            update_status("Running");
            while (running_) {
                if (!capture::capture_window_bgra(dpy, win, roi, frame, fw, fh) || frame.empty()) {
                    update_status("Capture failed (retrying)...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(120));
                    continue;
                }

                bool is_mvp = mvp_detector::is_mvp_screen(frame, fw, fh, roi, cfg.enable_ocr);
                auto gs = is_mvp ? state_machine::GameState::MVPShown : state_machine::GameState::InMatch;

                sm.update(gs, std::chrono::steady_clock::now());

                if (sm.should_send_message()) {
                    update_status("MVP detected! Sending message...");
                    window_finder::focus_window(dpy, win);
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));

                    // Open chat (assumes 'u' or 'y' are used in your config)
                    KeySym openChat = (cfg.chat_key == 'u') ? XK_u : XK_y;
                    keyboard::key_event(dpy, openChat, true);
                    keyboard::key_event(dpy, openChat, false);

                    std::this_thread::sleep_for(std::chrono::milliseconds(cfg.delay_ms));
                    keyboard::type_text(dpy, cfg.message, cfg.delay_ms);
                    std::this_thread::sleep_for(std::chrono::milliseconds(cfg.delay_ms));
                    keyboard::press_enter(dpy);

                    update_status("Message sent");
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            update_status("Stopped");
            XCloseDisplay(dpy);
        });
    }

    void stop_detection() {
        if (!running_) return;
        running_ = false;
        if (worker_.joinable()) worker_.join();
        update_status("Stopped");
    }
};

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    MainWindow win;
    win.show();
    gtk_main();
    return 0;
}