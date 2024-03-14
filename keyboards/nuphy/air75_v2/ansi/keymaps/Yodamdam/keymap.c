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
    A_ACCENT,
    E_ACCENT,
    I_ACCENT,
    U_ACCENT,
    O_ACCENT,
    CAPS_WORD
};

void register_hat_key(uint16_t key) {
    register_code(KC_RALT);
    register_code(KC_6);
    unregister_code(KC_6);
    unregister_code(KC_RALT);
    register_code(key);
    unregister_code(key);
}

void handle_hat_for_key(uint16_t key) {
    uint8_t mod_state = get_mods();
    if (mod_state == MOD_MASK_SHIFT) {
        del_mods(MOD_MASK_SHIFT);
        register_hat_key(key);
        set_mods(mod_state);
    } else {
        register_hat_key(key);
    };
}

td_state_t cur_dance(tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void a_finished(tap_dance_state_t *state, void *user_data);
void a_reset(tap_dance_state_t *state, void *user_data);
void e_finished(tap_dance_state_t *state, void *user_data);
void e_reset(tap_dance_state_t *state, void *user_data);
void i_finished(tap_dance_state_t *state, void *user_data);
void i_reset(tap_dance_state_t *state, void *user_data);
void u_finished(tap_dance_state_t *state, void *user_data);
void u_reset(tap_dance_state_t *state, void *user_data);
void o_finished(tap_dance_state_t *state, void *user_data);
void o_reset(tap_dance_state_t *state, void *user_data);
void caps_finished(tap_dance_state_t *state, void *user_data);
void capas_reset(tap_dance_state_t *state, void *user_data);

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

// Create an instance of 'td_tap_t' for the 'a' tap dance.
static td_tap_t atap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void a_finished(tap_dance_state_t *state, void *user_data) {
    atap_state.state = cur_dance(state);
    switch (atap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_Z);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            handle_hat_for_key(KC_A);
            break;
        default: break;
    }
}

void a_reset(tap_dance_state_t *state, void *user_data) {
    switch (atap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_Z);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
        default: break;
    }
    atap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'e' tap dance.
static td_tap_t etap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void e_finished(tap_dance_state_t *state, void *user_data) {
    etap_state.state = cur_dance(state);
    switch (etap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_G);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_F);
            break;
        case TD_SINGLE_HOLD:
            handle_hat_for_key(KC_E);
            break;
        default: break;
    }
}

void e_reset(tap_dance_state_t *state, void *user_data) {
    switch (etap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_G);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_F);
            break;
        case TD_SINGLE_HOLD:
        default: break;
    }
    etap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'e' tap dance.
static td_tap_t itap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void i_finished(tap_dance_state_t *state, void *user_data) {
    itap_state.state = cur_dance(state);
    switch (itap_state.state) {
        case TD_SINGLE_TAP:
            handle_hat_for_key(KC_I);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_I);
            break;
        default: break;
    }
}

void i_reset(tap_dance_state_t *state, void *user_data) {
    switch (itap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_I);
            break;
        default: break;
    }
    itap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'e' tap dance.
static td_tap_t utap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void u_finished(tap_dance_state_t *state, void *user_data) {
    utap_state.state = cur_dance(state);
    switch (utap_state.state) {
        case TD_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_H);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            handle_hat_for_key(KC_U);
            break;
        default: break;
    }
}

void u_reset(tap_dance_state_t *state, void *user_data) {
    switch (utap_state.state) {
        case TD_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_H);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            break;
        default: break;
    }
    utap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'e' tap dance.
static td_tap_t otap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void o_finished(tap_dance_state_t *state, void *user_data) {
    otap_state.state = cur_dance(state);
    switch (otap_state.state) {
        case TD_SINGLE_TAP:
            handle_hat_for_key(KC_O);
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            register_code(KC_RALT);
            register_code(KC_O);
            break;
        default: break;
    }
}

void o_reset(tap_dance_state_t *state, void *user_data) {
    switch (otap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_DOUBLE_TAP:
        case TD_DOUBLE_SINGLE_TAP:
            unregister_code(KC_RALT);
            unregister_code(KC_O);
            break;
        default: break;
    }
    otap_state.state = TD_NONE;
}

// Create an instance of 'td_tap_t' for the 'e' tap dance.
static td_tap_t caps_tap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void caps_finished(tap_dance_state_t *state, void *user_data) {
    caps_tap_state.state = cur_dance(state);
    switch (caps_tap_state.state) {
        case TD_SINGLE_TAP:
            caps_word_toggle();
            break;
        case TD_SINGLE_HOLD:
            layer_on(3);
            break;
        case TD_DOUBLE_SINGLE_TAP:
            caps_word_toggle();
            break;
        default: break;
    }
}

void caps_reset(tap_dance_state_t *state, void *user_data) {
    switch (caps_tap_state.state) {
        case TD_SINGLE_TAP:
            break;
        case TD_SINGLE_HOLD:
            layer_off(3);
            break;
        case TD_DOUBLE_SINGLE_TAP:
            break;
        default: break;
    }
    caps_tap_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [A_ACCENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, a_finished, a_reset),
    [E_ACCENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, e_finished, e_reset),
    [I_ACCENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, i_finished, i_reset),
    [U_ACCENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, u_finished, u_reset),
    [O_ACCENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, o_finished, o_reset),
    [CAPS_WORD] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, caps_finished, caps_reset),
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_SPC):
            return TAPPING_TERM * 3;
        case TD(CAPS_WORD):
            return TAPPING_TERM * 0.75;
        default:
            return TAPPING_TERM;
    }
}

// uint16_t get_autoshift_timeout(uint16_t keycode, keyrecord_t *record) {
//     switch(keycode) {
//         case KC_SPACE:
//             return 2 * get_generic_autoshift_timeout();
//         default:
//             return get_generic_autoshift_timeout();
//     }
// }

bool get_custom_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case KC_LEFT:
        case KC_RGHT:
        case KC_UP:
        case KC_DOWN:
        case KC_HOME:
        case KC_END:
            return true;
        default:
            return false;
    }
}

bool get_auto_shifted_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#    ifndef NO_AUTO_SHIFT_ALPHA
        case AUTO_SHIFT_ALPHA:
#    endif
#    ifndef NO_AUTO_SHIFT_NUMERIC
        case AUTO_SHIFT_NUMERIC:
#    endif
#    ifndef NO_AUTO_SHIFT_SPECIAL
#        ifndef NO_AUTO_SHIFT_TAB
        case KC_TAB:
#        endif
#        ifndef NO_AUTO_SHIFT_SYMBOLS
        case AUTO_SHIFT_SYMBOLS:
#        endif
#    endif
#    ifdef AUTO_SHIFT_ENTER
        case KC_ENT:
#    endif
            return true;
    }
    return get_custom_auto_shifted_key(keycode, record);
}

const uint16_t PROGMEM toggle_game_mode[] = {KC_LCTL, KC_LEFT, KC_RGHT, COMBO_END};
const uint16_t PROGMEM toggle_auto_shift[] = {KC_LCTL, KC_UP, KC_DOWN, COMBO_END};
combo_t key_combos[] = {
    COMBO(toggle_game_mode, TG(4)),
    COMBO(toggle_auto_shift, AS_TOGG),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// layer 0
[0] = LAYOUT_75_ansi(
    KC_ESC,             KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,         KC_PSCR,        KC_INS,         KC_DEL,
    KC_GRV,             KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,                         KC_BSPC,        KC_PGUP,
    KC_TAB,             KC_Q,           KC_D,           KC_T,           KC_R,           KC_W,           KC_Y,           KC_P,           KC_U,           KC_L,           KC_SCLN,        KC_LBRC,        KC_RBRC,                        KC_BSLS,        KC_PGDN,
    TD(CAPS_WORD),      KC_A,           KC_S,           KC_E,           KC_F,           KC_G,           KC_H,           KC_J,           KC_I,           KC_O,           KC_N,           KC_QUOT,                                        KC_ENT,         KC_HOME,
    SC_LSPO,                            KC_Z,           KC_X,           KC_C,           KC_V,           KC_K,           KC_B,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,                        SC_RSPC,        KC_UP,          KC_END,
    KC_LCTL,            KC_LGUI,        KC_LALT,                                                        LT(1, KC_SPC),                                                  MO(2),          MO(6),          KC_RCTL,        KC_LEFT,        KC_DOWN,        KC_RGHT),

// layer 1
[1] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            S(KC_7),        S(KC_BSLS),     S(KC_LBRC),     S(KC_RBRC),     XXXXXXX,        KC_PAST,        KC_7,           KC_8,           KC_9,           KC_COMM,        XXXXXXX,        XXXXXXX,                        _______,        _______,
    _______,            S(KC_COMM),     S(KC_DOT),      S(KC_9),        S(KC_0),        KC_MINS,        KC_PMNS,        KC_4,           KC_5,           KC_6,           KC_0,           KC_EQL,                                         _______,        _______,
    _______,                            XXXXXXX,        KC_LBRC,        KC_RBRC,        XXXXXXX,        KC_PSLS,        KC_PPLS,        KC_1,           KC_2,           KC_3,           KC_DOT,                         _______,        _______,        _______,
    _______,            _______,        _______,                                                        _______,                                                        _______,        _______,        _______,        _______,        _______,        _______),

// layer 2
[2] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        RALT(KC_5),     _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        TD(U_ACCENT),   _______,        _______,        _______,        _______,                        _______,        _______,
    _______,            TD(A_ACCENT),   _______,        TD(E_ACCENT),   _______,        _______,        _______,        _______,        TD(I_ACCENT),   TD(O_ACCENT),   _______,        _______,                                        _______,        _______,
    _______,                            _______,        _______,        RALT(KC_C),     _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,        _______,
    _______,            _______,        _______,                                                        _______,                                                        _______,        _______,        _______,        _______,        _______,        _______),

// layer 3
[3] = LAYOUT_75_ansi(
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,
    _______,            _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,        _______,                        _______,        _______,
    MT(MOD_LCTL,KC_TAB),_______,        LCA(KC_J),      C(KC_D),        C(KC_Y),        A(KC_INS),      XXXXXXX,        XXXXXXX,        KC_UP,          XXXXXXX,        XXXXXXX,        XXXXXXX,        XXXXXXX,                        _______,        _______,
    _______,            _______,        C(S(KC_N)),     MEH(KC_N),      C(S(KC_F)),     A(KC_J),        KC_HOME,        KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_END,         XXXXXXX,                                        _______,        _______,
    _______,                            LCA(KC_U),      LCA(KC_B),      KC_WH_L,        KC_WH_R,        XXXXXXX,        KC_WH_U,        KC_WH_D,        XXXXXXX,        XXXXXXX,        XXXXXXX,                        _______,        _______,        _______,
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
