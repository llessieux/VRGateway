#include "stdafx.h"
#include "PSPVRApp.h"
#include <sstream>
#include <istream>
#include <iostream>


typedef enum _keycode_t {
    NKCODE_BUTTON_CROSS = 23, // trackpad or X button(Xperia Play) is pressed
    NKCODE_BUTTON_CROSS_PS3 = 96, // PS3 X button is pressed
    NKCODE_BUTTON_CIRCLE = 1004, // Special custom keycode generated from 'O' button by our java code. Or 'O' button if Alt is pressed (TODO)
    NKCODE_BUTTON_CIRCLE_PS3 = 97, // PS3 O button is pressed
    NKCODE_BUTTON_SQUARE = 99, // Square button(Xperia Play) is pressed
    NKCODE_BUTTON_TRIANGLE = 100, // 'Triangle button(Xperia Play) is pressed
    NKCODE_DPAD_UP = 19,
    NKCODE_DPAD_DOWN = 20,
    NKCODE_DPAD_LEFT = 21,
    NKCODE_DPAD_RIGHT = 22,
    NKCODE_DPAD_CENTER = 23,
    NKCODE_UNKNOWN = 0,
    NKCODE_SOFT_LEFT = 1,
    NKCODE_SOFT_RIGHT = 2,
    NKCODE_HOME = 3,
    NKCODE_BACK = 4,
    NKCODE_CALL = 5,
    NKCODE_ENDCALL = 6,
    NKCODE_0 = 7,
    NKCODE_1 = 8,
    NKCODE_2 = 9,
    NKCODE_3 = 10,
    NKCODE_4 = 11,
    NKCODE_5 = 12,
    NKCODE_6 = 13,
    NKCODE_7 = 14,
    NKCODE_8 = 15,
    NKCODE_9 = 16,
    NKCODE_STAR = 17,
    NKCODE_POUND = 18,
    NKCODE_VOLUME_UP = 24,
    NKCODE_VOLUME_DOWN = 25,
    NKCODE_POWER = 26,
    NKCODE_CAMERA = 27,
    NKCODE_CLEAR = 28,
    NKCODE_A = 29,
    NKCODE_B = 30,
    NKCODE_C = 31,
    NKCODE_D = 32,
    NKCODE_E = 33,
    NKCODE_F = 34,
    NKCODE_G = 35,
    NKCODE_H = 36,
    NKCODE_I = 37,
    NKCODE_J = 38,
    NKCODE_K = 39,
    NKCODE_L = 40,
    NKCODE_M = 41,
    NKCODE_N = 42,
    NKCODE_O = 43,
    NKCODE_P = 44,
    NKCODE_Q = 45,
    NKCODE_R = 46,
    NKCODE_S = 47,
    NKCODE_T = 48,
    NKCODE_U = 49,
    NKCODE_V = 50,
    NKCODE_W = 51,
    NKCODE_X = 52,
    NKCODE_Y = 53,
    NKCODE_Z = 54,
    NKCODE_COMMA = 55,
    NKCODE_PERIOD = 56,
    NKCODE_ALT_LEFT = 57,
    NKCODE_ALT_RIGHT = 58,
    NKCODE_SHIFT_LEFT = 59,
    NKCODE_SHIFT_RIGHT = 60,
    NKCODE_TAB = 61,
    NKCODE_SPACE = 62,
    NKCODE_SYM = 63,
    NKCODE_EXPLORER = 64,
    NKCODE_ENVELOPE = 65,
    NKCODE_ENTER = 66,
    NKCODE_DEL = 67,
    NKCODE_GRAVE = 68,
    NKCODE_MINUS = 69,
    NKCODE_EQUALS = 70,
    NKCODE_LEFT_BRACKET = 71,
    NKCODE_RIGHT_BRACKET = 72,
    NKCODE_BACKSLASH = 73,
    NKCODE_SEMICOLON = 74,
    NKCODE_APOSTROPHE = 75,
    NKCODE_SLASH = 76,
    NKCODE_AT = 77,
    NKCODE_NUM = 78,
    NKCODE_HEADSETHOOK = 79,
    NKCODE_FOCUS = 80,
    NKCODE_PLUS = 81,
    NKCODE_MENU = 82,
    NKCODE_NOTIFICATION = 83,
    NKCODE_SEARCH = 84,
    NKCODE_MEDIA_PLAY_PAUSE = 85,
    NKCODE_MEDIA_STOP = 86,
    NKCODE_MEDIA_NEXT = 87,
    NKCODE_MEDIA_PREVIOUS = 88,
    NKCODE_MEDIA_REWIND = 89,
    NKCODE_MEDIA_FAST_FORWARD = 90,
    NKCODE_MUTE = 91,
    NKCODE_PAGE_UP = 92,
    NKCODE_PAGE_DOWN = 93,
    NKCODE_PICTSYMBOLS = 94,
    NKCODE_SWITCH_CHARSET = 95,
    NKCODE_BUTTON_A = 96,
    NKCODE_BUTTON_B = 97,
    NKCODE_BUTTON_C = 98,
    NKCODE_BUTTON_X = 99,
    NKCODE_BUTTON_Y = 100,
    NKCODE_BUTTON_Z = 101,
    NKCODE_BUTTON_L1 = 102,
    NKCODE_BUTTON_R1 = 103,
    NKCODE_BUTTON_L2 = 104,
    NKCODE_BUTTON_R2 = 105,
    NKCODE_BUTTON_THUMBL = 106,
    NKCODE_BUTTON_THUMBR = 107,
    NKCODE_BUTTON_START = 108,
    NKCODE_BUTTON_SELECT = 109,
    NKCODE_BUTTON_MODE = 110,
    NKCODE_ESCAPE = 111,
    NKCODE_FORWARD_DEL = 112,
    NKCODE_CTRL_LEFT = 113,
    NKCODE_CTRL_RIGHT = 114,
    NKCODE_CAPS_LOCK = 115,
    NKCODE_SCROLL_LOCK = 116,
    NKCODE_META_LEFT = 117,
    NKCODE_META_RIGHT = 118,
    NKCODE_FUNCTION = 119,
    NKCODE_SYSRQ = 120,
    NKCODE_BREAK = 121,
    NKCODE_MOVE_HOME = 122,
    NKCODE_MOVE_END = 123,
    NKCODE_INSERT = 124,
    NKCODE_FORWARD = 125,
    NKCODE_MEDIA_PLAY = 126,
    NKCODE_MEDIA_PAUSE = 127,
    NKCODE_MEDIA_CLOSE = 128,
    NKCODE_MEDIA_EJECT = 129,
    NKCODE_MEDIA_RECORD = 130,
    NKCODE_F1 = 131,
    NKCODE_F2 = 132,
    NKCODE_F3 = 133,
    NKCODE_F4 = 134,
    NKCODE_F5 = 135,
    NKCODE_F6 = 136,
    NKCODE_F7 = 137,
    NKCODE_F8 = 138,
    NKCODE_F9 = 139,
    NKCODE_F10 = 140,
    NKCODE_F11 = 141,
    NKCODE_F12 = 142,
    NKCODE_NUM_LOCK = 143,
    NKCODE_NUMPAD_0 = 144,
    NKCODE_NUMPAD_1 = 145,
    NKCODE_NUMPAD_2 = 146,
    NKCODE_NUMPAD_3 = 147,
    NKCODE_NUMPAD_4 = 148,
    NKCODE_NUMPAD_5 = 149,
    NKCODE_NUMPAD_6 = 150,
    NKCODE_NUMPAD_7 = 151,
    NKCODE_NUMPAD_8 = 152,
    NKCODE_NUMPAD_9 = 153,
    NKCODE_NUMPAD_DIVIDE = 154,
    NKCODE_NUMPAD_MULTIPLY = 155,
    NKCODE_NUMPAD_SUBTRACT = 156,
    NKCODE_NUMPAD_ADD = 157,
    NKCODE_NUMPAD_DOT = 158,
    NKCODE_NUMPAD_COMMA = 159,
    NKCODE_NUMPAD_ENTER = 160,
    NKCODE_NUMPAD_EQUALS = 161,
    NKCODE_NUMPAD_LEFT_PAREN = 162,
    NKCODE_NUMPAD_RIGHT_PAREN = 163,
    NKCODE_VOLUME_MUTE = 164,
    NKCODE_INFO = 165,
    NKCODE_CHANNEL_UP = 166,
    NKCODE_CHANNEL_DOWN = 167,
    NKCODE_ZOOM_IN = 168,
    NKCODE_ZOOM_OUT = 169,
    NKCODE_TV = 170,
    NKCODE_WINDOW = 171,
    NKCODE_GUIDE = 172,
    NKCODE_DVR = 173,
    NKCODE_BOOKMARK = 174,
    NKCODE_CAPTIONS = 175,
    NKCODE_SETTINGS = 176,
    NKCODE_TV_POWER = 177,
    NKCODE_TV_INPUT = 178,
    NKCODE_STB_POWER = 179,
    NKCODE_STB_INPUT = 180,
    NKCODE_AVR_POWER = 181,
    NKCODE_AVR_INPUT = 182,
    NKCODE_PROG_RED = 183,
    NKCODE_PROG_GREEN = 184,
    NKCODE_PROG_YELLOW = 185,
    NKCODE_PROG_BLUE = 186,
    NKCODE_APP_SWITCH = 187,
    NKCODE_BUTTON_1 = 188,
    NKCODE_BUTTON_2 = 189,
    NKCODE_BUTTON_3 = 190,
    NKCODE_BUTTON_4 = 191,
    NKCODE_BUTTON_5 = 192,
    NKCODE_BUTTON_6 = 193,
    NKCODE_BUTTON_7 = 194,
    NKCODE_BUTTON_8 = 195,
    NKCODE_BUTTON_9 = 196,
    NKCODE_BUTTON_10 = 197,
    NKCODE_BUTTON_11 = 198,
    NKCODE_BUTTON_12 = 199,
    NKCODE_BUTTON_13 = 200,
    NKCODE_BUTTON_14 = 201,
    NKCODE_BUTTON_15 = 202,
    NKCODE_BUTTON_16 = 203,
    NKCODE_LANGUAGE_SWITCH = 204,
    NKCODE_MANNER_MODE = 205,
    NKCODE_3D_MODE = 206,
    NKCODE_CONTACTS = 207,
    NKCODE_CALENDAR = 208,
    NKCODE_MUSIC = 209,
    NKCODE_CALCULATOR = 210,
    NKCODE_ZENKAKU_HANKAKU = 211,
    NKCODE_EISU = 212,
    NKCODE_MUHENKAN = 213,
    NKCODE_HENKAN = 214,
    NKCODE_KATAKANA_HIRAGANA = 215,
    NKCODE_YEN = 216,
    NKCODE_RO = 217,
    NKCODE_KANA = 218,
    NKCODE_ASSIST = 219,

    // Extended keycodes on latam kayboards.

    NKCODE_START_QUESTION = 220,
    NKCODE_LEFTBRACE = 221,
    NKCODE_RIGHTBRACE = 222,

    // Ouya buttons. Just here for reference, they map straight to regular android buttons
    // and will be mapped the same way.
    NKCODE_OUYA_BUTTON_A = 97,
    NKCODE_OUYA_BUTTON_DPAD_DOWN = 20,
    NKCODE_OUYA_BUTTON_DPAD_LEFT = 21,
    NKCODE_OUYA_BUTTON_DPAD_RIGHT = 22,
    NKCODE_OUYA_BUTTON_DPAD_UP = 19,
    NKCODE_OUYA_BUTTON_L1 = 102,
    NKCODE_OUYA_BUTTON_L2 = 104,
    NKCODE_OUYA_BUTTON_L3 = 106,
    NKCODE_OUYA_BUTTON_MENU = 82,
    NKCODE_OUYA_BUTTON_O = 96,
    NKCODE_OUYA_BUTTON_R1 = 103,
    NKCODE_OUYA_BUTTON_R2 = 105,
    NKCODE_OUYA_BUTTON_R3 = 107,
    NKCODE_OUYA_BUTTON_U = 99,
    NKCODE_OUYA_BUTTON_Y = 100,

    // Extended keycodes, not available on Android
    NKCODE_EXT_PIPE = 1001,  // The key next to Z on euro 102-key keyboards.

    NKCODE_EXT_MOUSEBUTTON_1 = 1002,
    NKCODE_EXT_MOUSEBUTTON_2 = 1003,
    NKCODE_EXT_MOUSEBUTTON_3 = 1004,
    NKCODE_EXT_MOUSEWHEEL_UP = 1008,
    NKCODE_EXT_MOUSEWHEEL_DOWN = 1009
} keycode_t;

std::map<int, int> windowsTransTable = {
    { 'A', NKCODE_A },
    { 'B', NKCODE_B },
    { 'C', NKCODE_C },
    { 'D', NKCODE_D },
    { 'E', NKCODE_E },
    { 'F', NKCODE_F },
    { 'G', NKCODE_G },
    { 'H', NKCODE_H },
    { 'I', NKCODE_I },
    { 'J', NKCODE_J },
    { 'K', NKCODE_K },
    { 'L', NKCODE_L },
    { 'M', NKCODE_M },
    { 'N', NKCODE_N },
    { 'O', NKCODE_O },
    { 'P', NKCODE_P },
    { 'Q', NKCODE_Q },
    { 'R', NKCODE_R },
    { 'S', NKCODE_S },
    { 'T', NKCODE_T },
    { 'U', NKCODE_U },
    { 'V', NKCODE_V },
    { 'W', NKCODE_W },
    { 'X', NKCODE_X },
    { 'Y', NKCODE_Y },
    { 'Z', NKCODE_Z },
    { '0', NKCODE_0 },
    { '1', NKCODE_1 },
    { '2', NKCODE_2 },
    { '3', NKCODE_3 },
    { '4', NKCODE_4 },
    { '5', NKCODE_5 },
    { '6', NKCODE_6 },
    { '7', NKCODE_7 },
    { '8', NKCODE_8 },
    { '9', NKCODE_9 },
    { VK_OEM_PERIOD, NKCODE_PERIOD },
    { VK_OEM_COMMA, NKCODE_COMMA },
    { VK_NUMPAD0, NKCODE_NUMPAD_0 },
    { VK_NUMPAD1, NKCODE_NUMPAD_1 },
    { VK_NUMPAD2, NKCODE_NUMPAD_2 },
    { VK_NUMPAD3, NKCODE_NUMPAD_3 },
    { VK_NUMPAD4, NKCODE_NUMPAD_4 },
    { VK_NUMPAD5, NKCODE_NUMPAD_5 },
    { VK_NUMPAD6, NKCODE_NUMPAD_6 },
    { VK_NUMPAD7, NKCODE_NUMPAD_7 },
    { VK_NUMPAD8, NKCODE_NUMPAD_8 },
    { VK_NUMPAD9, NKCODE_NUMPAD_9 },
    { VK_DECIMAL, NKCODE_NUMPAD_DOT },
    { VK_DIVIDE, NKCODE_NUMPAD_DIVIDE },
    { VK_MULTIPLY, NKCODE_NUMPAD_MULTIPLY },
    { VK_SUBTRACT, NKCODE_NUMPAD_SUBTRACT },
    { VK_ADD, NKCODE_NUMPAD_ADD },
    { VK_SEPARATOR, NKCODE_NUMPAD_COMMA },
    { VK_OEM_MINUS, NKCODE_MINUS },
    { VK_OEM_PLUS, NKCODE_PLUS },
    { VK_LCONTROL, NKCODE_CTRL_LEFT },
    { VK_RCONTROL, NKCODE_CTRL_RIGHT },
    { VK_LSHIFT, NKCODE_SHIFT_LEFT },
    { VK_RSHIFT, NKCODE_SHIFT_RIGHT },
    { VK_LMENU, NKCODE_ALT_LEFT },
    { VK_RMENU, NKCODE_ALT_RIGHT },
    { VK_BACK, NKCODE_DEL },  // yes! http://stackoverflow.com/questions/4886858/android-edittext-deletebackspace-key-event
    { VK_SPACE, NKCODE_SPACE },
    { VK_ESCAPE, NKCODE_ESCAPE },
    { VK_UP, NKCODE_DPAD_UP },
    { VK_INSERT, NKCODE_INSERT },
    { VK_HOME, NKCODE_MOVE_HOME },
    { VK_PRIOR, NKCODE_PAGE_UP },
    { VK_NEXT, NKCODE_PAGE_DOWN },
    { VK_DELETE, NKCODE_FORWARD_DEL },
    { VK_END, NKCODE_MOVE_END },
    { VK_TAB, NKCODE_TAB },
    { VK_DOWN, NKCODE_DPAD_DOWN },
    { VK_LEFT, NKCODE_DPAD_LEFT },
    { VK_RIGHT, NKCODE_DPAD_RIGHT },
    { VK_CAPITAL, NKCODE_CAPS_LOCK },
    { VK_CLEAR, NKCODE_CLEAR },
    { VK_SNAPSHOT, NKCODE_SYSRQ },
    { VK_SCROLL, NKCODE_SCROLL_LOCK },
    { VK_OEM_1, NKCODE_SEMICOLON },
    { VK_OEM_2, NKCODE_SLASH },
    { VK_OEM_3, NKCODE_GRAVE },
    { VK_OEM_4, NKCODE_LEFT_BRACKET },
    { VK_OEM_5, NKCODE_BACKSLASH },
    { VK_OEM_6, NKCODE_RIGHT_BRACKET },
    { VK_OEM_7, NKCODE_APOSTROPHE },
    { VK_RETURN, NKCODE_ENTER },
    { VK_APPS, NKCODE_MENU }, // Context menu key, let's call this "menu".
    { VK_PAUSE, NKCODE_BREAK },
    { VK_F1, NKCODE_F1 },
    { VK_F2, NKCODE_F2 },
    { VK_F3, NKCODE_F3 },
    { VK_F4, NKCODE_F4 },
    { VK_F5, NKCODE_F5 },
    { VK_F6, NKCODE_F6 },
    { VK_F7, NKCODE_F7 },
    { VK_F8, NKCODE_F8 },
    { VK_F9, NKCODE_F9 },
    { VK_F10, NKCODE_F10 },
    { VK_F11, NKCODE_F11 },
    { VK_F12, NKCODE_F12 },
    { VK_OEM_102, NKCODE_EXT_PIPE },
    { VK_LBUTTON, NKCODE_EXT_MOUSEBUTTON_1 },
    { VK_RBUTTON, NKCODE_EXT_MOUSEBUTTON_2 } };


void SplitString(const std::string& str, const char delim, std::vector<std::string>& output)
{
    std::istringstream iss(str);
    output.resize(1);

    while (std::getline(iss, *output.rbegin(), delim))
        output.push_back("");

    output.pop_back();
}

PSPVRApp::PSPVRApp()
{
    m_keys = std::vector<std::string>{
        "Left", "Up", "Right", "Down", "L", "An.Up",
        "Square", "Triangle", "Circle", "Cross", "R", "An.Left"
    };
}


PSPVRApp::~PSPVRApp()
{
}



const wchar_t *PSPVRApp::GetSourceParentClassName() const
{
    return L"PPSSPPWnd";
}

const wchar_t *PSPVRApp::GetSourceParentWindowName() const
{
    return nullptr;
}

const wchar_t *PSPVRApp::GetSourceClassName() const
{
    return L"PPSSPPDisplay";
}
const wchar_t *PSPVRApp::GetSourceWindowName() const
{
    return nullptr;
}

void PSPVRApp::ProcessButton(const ControllerID device, const vr::VRControllerState_t &state)
{
    std::array<vr::EVRButtonId, 6> buttons = { 
        vr::k_EButton_DPad_Left, vr::k_EButton_DPad_Up , vr::k_EButton_DPad_Right, vr::k_EButton_DPad_Down, vr::k_EButton_A, vr::k_EButton_ApplicationMenu };

    FindKeyboard();

    SetFocus(m_source_parent_window);

    uint64_t pressed_buttons = ButtonsFromState(state);
    for (int i = 0; i < 6; i++)
    {
        uint64_t mask = vr::ButtonMaskFromId(buttons[i]);
        uint64_t current_button_state = pressed_buttons & mask;
        uint64_t previous_button_state = m_prev_state.at(device) & mask;
        if (current_button_state && !previous_button_state)
        {
            SendKey(WM_KEYDOWN, m_button_to_key_map[m_keys[device * 6 + i]]);
        }
        if (!current_button_state && previous_button_state)
        {
            SendKey(WM_KEYUP, m_button_to_key_map[m_keys[device * 6 + i]]);
        }
    }

    m_prev_state.at(device) = pressed_buttons;
}



void PSPVRApp::ReadPSPControlSettings(const char *pspControlsIniFileName)
{
    char *control_key[2][7] = {
        { "Left", "Up", "Right", "Down", "L", "An.Up", "Select" },
        { "Square", "Triangle", "Circle", "Cross", "R", "An.Left", "Start" }
    };

    FILE *f = nullptr;
    fopen_s(&f, pspControlsIniFileName, "r");
    if (f == nullptr)
        return;

    char txt[256] = { 0 };
    while (!feof(f))
    {
        memset(txt, 0, 256);
        fgets(txt, 256, f);

        bool found = false;
        char *key_value = strchr(txt, '=');
        if (key_value == nullptr)
            continue;
        *key_value = 0;
        for (char *ptr = key_value - 1; ptr > txt; ptr--)
        {
            if (*ptr == ' ')
                *ptr = 0;
            else
                break;
        }

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                if (strcmp(txt, control_key[i][j]) == 0)
                {
                    found = true;
                    key_value++;
                    std::vector<std::string> outs;
                    SplitString(std::string(key_value), ',', outs);
                    for (const auto &v : outs)
                    {
                        if (strncmp(v.c_str(), " 1-", 3) == 00)
                        {
                            int key = atoi(v.c_str() + 3);
                            for (const auto & map_entry : windowsTransTable)
                            {
                                if (map_entry.second == key)
                                {
                                    m_button_to_key_map[std::string(control_key[i][j])] = map_entry.first;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            if (found)
                break;
        }
    }
    fclose(f);
}
