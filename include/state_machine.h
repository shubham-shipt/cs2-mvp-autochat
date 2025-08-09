#pragma once
#include <string>
#include <chrono>
namespace state_machine {
enum class GameState {
    Unknown,
    InMatch,
    RoundEnd,
    MVPShown,
    Paused
};
class StateMachine {
public:
    StateMachine();
    void update(GameState new_state, std::chrono::steady_clock::time_point now);
    GameState current_state() const;
    bool should_send_message() const;
    void reset();
    void set_debounce_ms(int ms);
    std::string state_str() const;
private:
    GameState state_;
    std::chrono::steady_clock::time_point last_mvp_;
    int debounce_ms_;
    bool send_flag_;
};
}