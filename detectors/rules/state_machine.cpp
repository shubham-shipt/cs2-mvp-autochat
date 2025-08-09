#include "state_machine.h"
#include <chrono>
#include <string>

namespace state_machine {

StateMachine::StateMachine()
    : state_(GameState::Unknown), debounce_ms_(500), send_flag_(false) {
    last_mvp_ = std::chrono::steady_clock::now() - std::chrono::milliseconds(10000);
}

void StateMachine::update(GameState new_state, std::chrono::steady_clock::time_point now) {
    if (state_ != GameState::MVPShown && new_state == GameState::MVPShown) {
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_mvp_).count();
        if (dt > debounce_ms_) {
            send_flag_ = true;
            last_mvp_ = now;
        } else {
            send_flag_ = false;
        }
    } else {
        send_flag_ = false;
    }
    state_ = new_state;
}

GameState StateMachine::current_state() const {
    return state_;
}

bool StateMachine::should_send_message() const {
    return send_flag_;
}

void StateMachine::reset() {
    state_ = GameState::Unknown;
    last_mvp_ = std::chrono::steady_clock::now() - std::chrono::milliseconds(10000);
    send_flag_ = false;
}

void StateMachine::set_debounce_ms(int ms) {
    debounce_ms_ = ms;
}

std::string StateMachine::state_str() const {
    switch (state_) {
        case GameState::Unknown:   return "Unknown";
        case GameState::InMatch:   return "InMatch";
        case GameState::RoundEnd:  return "RoundEnd";
        case GameState::MVPShown:  return "MVPShown";
        case GameState::Paused:    return "Paused";
    }
    return "Unknown";
}

}