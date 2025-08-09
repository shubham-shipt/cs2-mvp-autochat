#include "keyboard.h"
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#include <map>

namespace keyboard {

bool key_event(Display* dpy, KeySym sym, bool press) {
    KeyCode code = XKeysymToKeycode(dpy, sym);
    if (!code) return false;
    XTestFakeKeyEvent(dpy, code, press ? True : False, CurrentTime);
    XFlush(dpy);
    return true;
}

bool map_char(char c, KeySym& sym, bool& need_shift) {
    need_shift = false;
    static std::map<char, KeySym> no_shift = {
        {'a', XK_a},{'b', XK_b},{'c', XK_c},{'d', XK_d},{'e', XK_e},{'f', XK_f},
        {'g', XK_g},{'h', XK_h},{'i', XK_i},{'j', XK_j},{'k', XK_k},{'l', XK_l},
        {'m', XK_m},{'n', XK_n},{'o', XK_o},{'p', XK_p},{'q', XK_q},{'r', XK_r},
        {'s', XK_s},{'t', XK_t},{'u', XK_u},{'v', XK_v},{'w', XK_w},{'x', XK_x},
        {'y', XK_y},{'z', XK_z},{'0', XK_0},{'1', XK_1},{'2', XK_2},{'3', XK_3},
        {'4', XK_4},{'5', XK_5},{'6', XK_6},{'7', XK_7},{'8', XK_8},{'9', XK_9},
        {' ', XK_space},{'-', XK_minus},{'=', XK_equal},{'[', XK_bracketleft},
        {']', XK_bracketright},{';', XK_semicolon},{'\'', XK_apostrophe},{',', XK_comma},
        {'.', XK_period},{'/', XK_slash},{'\\', XK_backslash},{'`', XK_grave}
    };
    static std::map<char, std::pair<KeySym,bool>> shift_map = {
        {'A', {XK_a,true}}, {'B', {XK_b,true}}, {'C', {XK_c,true}}, {'D', {XK_d,true}}, {'E', {XK_e,true}},
        {'F', {XK_f,true}}, {'G', {XK_g,true}}, {'H', {XK_h,true}}, {'I', {XK_i,true}}, {'J', {XK_j,true}},
        {'K', {XK_k,true}}, {'L', {XK_l,true}}, {'M', {XK_m,true}}, {'N', {XK_n,true}}, {'O', {XK_o,true}},
        {'P', {XK_p,true}}, {'Q', {XK_q,true}}, {'R', {XK_r,true}}, {'S', {XK_s,true}}, {'T', {XK_t,true}},
        {'U', {XK_u,true}}, {'V', {XK_v,true}}, {'W', {XK_w,true}}, {'X', {XK_x,true}}, {'Y', {XK_y,true}},
        {'Z', {XK_z,true}},
        {'!', {XK_1,true}}, {'@', {XK_2,true}}, {'#', {XK_3,true}}, {'$', {XK_4,true}}, {'%', {XK_5,true}},
        {'^', {XK_6,true}}, {'&', {XK_7,true}}, {'*', {XK_8,true}}, {'(', {XK_9,true}}, {')', {XK_0,true}},
        {'_', {XK_minus,true}}, {'+', {XK_equal,true}}, {'{', {XK_bracketleft,true}}, {'}', {XK_bracketright,true}},
        {':', {XK_semicolon,true}}, {'"', {XK_apostrophe,true}}, {'<', {XK_comma,true}}, {'>', {XK_period,true}},
        {'?', {XK_slash,true}}, {'|', {XK_backslash,true}}, {'~', {XK_grave,true}}
    };
    if (no_shift.count(c)) { sym = no_shift[c]; return true; }
    if (shift_map.count(c)) { sym = shift_map[c].first; need_shift = shift_map[c].second; return true; }
    return false;
}

bool type_text(Display* dpy, const std::string& text, int delay_ms) {
    for (char c : text) {
        KeySym sym; bool shift;
        if (!map_char(c, sym, shift)) continue;
        if (shift) key_event(dpy, XK_Shift_L, true);
        key_event(dpy, sym, true);
        usleep(3000);
        key_event(dpy, sym, false);
        if (shift) key_event(dpy, XK_Shift_L, false);
        usleep(delay_ms * 1000);
    }
    return true;
}

bool type_keys(Display* dpy, const std::vector<KeySym>& syms, int delay_ms) {
    for (KeySym s : syms) {
        key_event(dpy, s, true);
        usleep(2000);
        key_event(dpy, s, false);
        usleep(delay_ms * 1000);
    }
    return true;
}

bool press_enter(Display* dpy) {
    return key_event(dpy, XK_Return, true) && key_event(dpy, XK_Return, false);
}

}