/*
Copyright 2023 @ Nuphy <https://nuphy.com/>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "keymap_us_international_linux.h"

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    TD_MINS,
    TD_PPLS,
};

td_state_t cur_dance(tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void minus_finished(tap_dance_state_t *state, void *user_data);
void minus_reset(tap_dance_state_t *state, void *user_data);
void plus_finished(tap_dance_state_t *state, void *user_data);
void plus_reset(tap_dance_state_t *state, void *user_data);

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'minus-divide' tap dance.
static td_tap_t minus_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void minus_finished(tap_dance_state_t *state, void *user_data) {
    minus_tap_state.state = cur_dance(state);
    switch (minus_tap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_MINS);
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_SLSH);
            break;
        default: break;
    }
}

void minus_reset(tap_dance_state_t *state, void *user_data) {
    switch (minus_tap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_MINS);
            break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_SLSH);
            break;
        default: break;
    }
    minus_tap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'plus-times' tap dance.
static td_tap_t plus_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void plus_finished(tap_dance_state_t *state, void *user_data) {
    plus_tap_state.state = cur_dance(state);
    switch (plus_tap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_PPLS);
            break;
        case TD_SINGLE_HOLD:
            register_code(KC_PAST);
            break;
        default: break;
    }
}

void plus_reset(tap_dance_state_t *state, void *user_data) {
    switch (plus_tap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_PPLS);
            break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_PAST);
            break;
        default: break;
    }
    plus_tap_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_MINS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, minus_finished, minus_reset),
    [TD_PPLS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, plus_finished, plus_reset),
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_SPC):
            return TAPPING_TERM * 1.5;
        default:
            return TAPPING_TERM;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// layer 0
[0] = LAYOUT_75_ansi(
    KC_ESC,             KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         KC_PSCR,        KC_INS,         KC_DEL,
    KC_GRV,             KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,                         KC_BSPC,        KC_PGUP,
    KC_TAB,             KC_Q,           KC_D,           KC_T,           KC_R,           KC_W,           KC_Y,           KC_P,           KC_U,           KC_L,           KC_SCLN,        KC_LBRC,        KC_RBRC,                        KC_BSLS,        KC_PGDN,
    LT(3, KC_CAPS),     KC_A,           ALT_T(KC_S),    CTL_T(KC_E),    SFT_T(KC_F),    KC_G,           KC_H,           SFT_T(KC_J),    CTL_T(KC_I),    ALT_T(KC_O),    KC_N,           KC_QUOT,                                        KC_ENT,         KC_HOME,
    KC_LSFT,                            KC_Z,           KC_X,           KC_C,           KC_V,           KC_K,           KC_B,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,                        KC_RSFT,        KC_UP,          KC_END,
    KC_LCTL,            KC_LGUI,        KC_LALT,                                                        LT(1, KC_SPC),                                                  KC_RALT,        MO(6),          KC_RCTL,        KC_LEFT,        KC_DOWN,        KC_RGHT),

// layer 1
[1] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            KC_7,           KC_BSLS,        KC_LBRC,        KC_RBRC,        XXXXXXX,        KC_SLSH,        KC_7,           KC_8,           KC_9,           KC_COMM,        XXXXXXX,        XXXXXXX,                        _______,        _______,
    _______,            KC_COMM,        ALT_T(KC_DOT),  CTL_T(KC_9),    SFT_T(KC_0),    KC_MINS,        TD(TD_MINS),    KC_4,           KC_5,           KC_6,           KC_0,           KC_EQL,                                         _______,        _______,
    _______,                            XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        TD(TD_PPLS),    KC_1,           KC_2,           KC_3,           KC_DOT,                         _______,        _______,        _______,
    _______,            _______,        _______,                                                        _______,                                                        KC_LSFT,        _______,        _______,        _______,        _______,        _______),

// layer 2
[2] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                                        _______,        _______,
    _______,                            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,        _______,
    _______,            _______,        _______,                                                        _______,                                                        _______,        _______,        _______,        _______,        _______,        _______),

// layer 3
[3] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            XXXXXXX,        XXXXXXX,        KC_TAB,         XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        KC_UP,          XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                        _______,        _______,
    _______,            XXXXXXX,        KC_LALT,        KC_LCTL,        KC_LSFT,        XXXXXXX,        KC_HOME,        KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_END,         XXXXXXX,                                        _______,        _______,
    _______,                            XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                        _______,        _______,        _______,
    _______,            _______,        _______,                                                        _______,                                                        _______,        _______,        _______,        _______,        _______,        _______),

// layer 4
[4] = LAYOUT_75_ansi(
    KC_ESC,             KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         KC_PSCR,        KC_INS,         KC_DEL,
    KC_GRV,             KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,                         KC_BSPC,        KC_PGUP,
    KC_TAB,             KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_LBRC,        KC_RBRC,                        KC_BSLS,        KC_PGDN,
    KC_CAPS,            KC_A,           KC_S,           KC_D,           KC_F,           KC_G,           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_QUOT,                                        KC_ENT,         KC_HOME,
    KC_LSFT,                            KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,           KC_N,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,                        KC_RSFT,        KC_UP,          KC_END,
    KC_LCTL,            KC_LGUI,        KC_LALT,                                                        KC_SPC,                                                         KC_RALT,        MO(6),          KC_RCTL,        KC_LEFT,        KC_DOWN,        KC_RGHT),

// layer 5
[5] = LAYOUT_75_ansi(
    KC_ESC,             KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         KC_PSCR,        KC_INS,         KC_DEL,
    KC_GRV,             KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,                         KC_BSPC,        KC_PGUP,
    KC_TAB,             KC_Q,           KC_D,           KC_T,           KC_R,           KC_W,           KC_Y,           KC_P,           KC_U,           KC_L,           KC_SCLN,        KC_LBRC,        KC_RBRC,                        KC_BSLS,        KC_PGDN,
    KC_CAPS,            KC_A,           KC_S,           KC_E,           KC_F,           KC_G,           KC_H,           KC_J,           KC_I,           KC_O,           KC_N,           KC_QUOT,                                        KC_ENT,         KC_HOME,
    KC_LSFT,                            KC_Z,           KC_X,           KC_C,           KC_V,           KC_K,           KC_B,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,                        KC_RSFT,        KC_UP,          KC_END,
    KC_LCTL,            KC_LGUI,        KC_LALT,                                                        KC_SPC,                                                         KC_RALT,        MO(6),          KC_RCTL,        KC_LEFT,        KC_DOWN,        KC_RGHT),


// layer 6
[6] = LAYOUT_75_ansi(
    KC_LNUM,            KC_BRID,        KC_BRIU,        KC_WSCH,        KC_FIND,        PB_1,           G(KC_L),        KC_MPRV,        KC_MPLY,        KC_MNXT,        KC_MUTE,        KC_VOLD,        KC_VOLU,        _______,        SIDE_MOD,       SIDE_HUI,
    _______,            LNK_BLE1,       LNK_BLE2,       LNK_BLE3,       LNK_RF,         _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        SIDE_SPI,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        DEV_RESET,      SLEEP_MODE,                     BAT_SHOW,       SIDE_SPD,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                                        _______,        SIDE_VAI,
    _______,                            _______,        _______,        RGB_TEST,       _______,        BAT_NUM,        _______,        _______,        RGB_SPD,        RGB_SPI,        _______,                        _______,        RGB_VAI,        SIDE_VAD,
    _______,            _______,        _______,                                                        _______,                                        _______,        _______,        _______,                        RGB_MOD,        RGB_VAD,        RGB_HUI)

//// layer X
// [X] = LAYOUT_75_ansi(
    // _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    // _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    // _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    // _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                                        _______,        _______,
    // _______,                            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,        _______,
    // _______,            _______,        _______,                                                        _______,                                                        _______,        _______,        _______,        _______,        _______,        _______),
};
