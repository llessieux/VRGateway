#include "stdafx.h"
#include "camera.h"
#include "Model.h"
#include "Screen.h"
#include "colorshader.h"
#include "renderTexture.h"
#include "debugwindow.h"
#include "DesktopGrabber.h"
#include "TaikoVRApp.h"
#include <map>
#include <sstream>
#include <istream>
#include <iostream>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "openvr_api.lib")
#pragma optimize ("",off)
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float MOVE_STEP = 0.3f;
const float ROTATE_STEP = 5;


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
{'A', NKCODE_A},
{'B', NKCODE_B},
{'C', NKCODE_C},
{'D', NKCODE_D},
{'E', NKCODE_E},
{'F', NKCODE_F},
{'G', NKCODE_G},
{'H', NKCODE_H},
{'I', NKCODE_I},
{'J', NKCODE_J},
{'K', NKCODE_K},
{'L', NKCODE_L},
{'M', NKCODE_M},
{'N', NKCODE_N},
{'O', NKCODE_O},
{'P', NKCODE_P},
{'Q', NKCODE_Q},
{'R', NKCODE_R},
{'S', NKCODE_S},
{'T', NKCODE_T},
{'U', NKCODE_U},
{'V', NKCODE_V},
{'W', NKCODE_W},
{'X', NKCODE_X},
{'Y', NKCODE_Y},
{'Z', NKCODE_Z},
{'0', NKCODE_0},
{'1', NKCODE_1},
{'2', NKCODE_2},
{'3', NKCODE_3},
{'4', NKCODE_4},
{'5', NKCODE_5},
{'6', NKCODE_6},
{'7', NKCODE_7},
{'8', NKCODE_8},
{'9', NKCODE_9},
{VK_OEM_PERIOD, NKCODE_PERIOD},
{VK_OEM_COMMA, NKCODE_COMMA},
{VK_NUMPAD0, NKCODE_NUMPAD_0},
{VK_NUMPAD1, NKCODE_NUMPAD_1},
{VK_NUMPAD2, NKCODE_NUMPAD_2},
{VK_NUMPAD3, NKCODE_NUMPAD_3},
{VK_NUMPAD4, NKCODE_NUMPAD_4},
{VK_NUMPAD5, NKCODE_NUMPAD_5},
{VK_NUMPAD6, NKCODE_NUMPAD_6},
{VK_NUMPAD7, NKCODE_NUMPAD_7},
{VK_NUMPAD8, NKCODE_NUMPAD_8},
{VK_NUMPAD9, NKCODE_NUMPAD_9},
{VK_DECIMAL, NKCODE_NUMPAD_DOT},
{VK_DIVIDE, NKCODE_NUMPAD_DIVIDE},
{VK_MULTIPLY, NKCODE_NUMPAD_MULTIPLY},
{VK_SUBTRACT, NKCODE_NUMPAD_SUBTRACT},
{VK_ADD, NKCODE_NUMPAD_ADD},
{VK_SEPARATOR, NKCODE_NUMPAD_COMMA},
{VK_OEM_MINUS, NKCODE_MINUS},
{VK_OEM_PLUS, NKCODE_PLUS},
{VK_LCONTROL, NKCODE_CTRL_LEFT},
{VK_RCONTROL, NKCODE_CTRL_RIGHT},
{VK_LSHIFT, NKCODE_SHIFT_LEFT},
{VK_RSHIFT, NKCODE_SHIFT_RIGHT},
{VK_LMENU, NKCODE_ALT_LEFT},
{VK_RMENU, NKCODE_ALT_RIGHT},
{VK_BACK, NKCODE_DEL},  // yes! http://stackoverflow.com/questions/4886858/android-edittext-deletebackspace-key-event
{VK_SPACE, NKCODE_SPACE},
{VK_ESCAPE, NKCODE_ESCAPE},
{VK_UP, NKCODE_DPAD_UP},
{VK_INSERT, NKCODE_INSERT},
{VK_HOME, NKCODE_MOVE_HOME},
{VK_PRIOR, NKCODE_PAGE_UP},
{VK_NEXT, NKCODE_PAGE_DOWN},
{VK_DELETE, NKCODE_FORWARD_DEL},
{VK_END, NKCODE_MOVE_END},
{VK_TAB, NKCODE_TAB},
{VK_DOWN, NKCODE_DPAD_DOWN},
{VK_LEFT, NKCODE_DPAD_LEFT},
{VK_RIGHT, NKCODE_DPAD_RIGHT},
{VK_CAPITAL, NKCODE_CAPS_LOCK},
{VK_CLEAR, NKCODE_CLEAR},
{VK_SNAPSHOT, NKCODE_SYSRQ},
{VK_SCROLL, NKCODE_SCROLL_LOCK},
{VK_OEM_1, NKCODE_SEMICOLON},
{VK_OEM_2, NKCODE_SLASH},
{VK_OEM_3, NKCODE_GRAVE},
{VK_OEM_4, NKCODE_LEFT_BRACKET},
{VK_OEM_5, NKCODE_BACKSLASH},
{VK_OEM_6, NKCODE_RIGHT_BRACKET},
{VK_OEM_7, NKCODE_APOSTROPHE},
{VK_RETURN, NKCODE_ENTER},
{VK_APPS, NKCODE_MENU}, // Context menu key, let's call this "menu".
{VK_PAUSE, NKCODE_BREAK},
{VK_F1, NKCODE_F1},
{VK_F2, NKCODE_F2},
{VK_F3, NKCODE_F3},
{VK_F4, NKCODE_F4},
{VK_F5, NKCODE_F5},
{VK_F6, NKCODE_F6},
{VK_F7, NKCODE_F7},
{VK_F8, NKCODE_F8},
{VK_F9, NKCODE_F9},
{VK_F10, NKCODE_F10},
{VK_F11, NKCODE_F11},
{VK_F12, NKCODE_F12},
{VK_OEM_102, NKCODE_EXT_PIPE},
{VK_LBUTTON, NKCODE_EXT_MOUSEBUTTON_1},
{VK_RBUTTON, NKCODE_EXT_MOUSEBUTTON_2} };

BaseVRApp::BaseVRApp() :
    m_source_parent_window(0),
    m_source_window(0),
    m_errorshown(false),
    m_keyboard_handle(0)
{
    m_keys = std::vector<std::string>{
        "Left", "Up", "Right", "Down", "L", "An.Up",
        "Square", "Triangle", "Circle", "Cross", "R", "An.Left"
    };
    m_background_colors[0] = 0.0f;
    m_background_colors[1] = 0.0f;
    m_background_colors[2] = 1.0f;
}


BaseVRApp::~BaseVRApp()
{
    clean();
}

void SplitString(const std::string& str, const char delim, std::vector<std::string>& output)
{
    std::istringstream iss(str);
    output.resize(1);

    while (std::getline(iss, *output.rbegin(), delim))
        output.push_back("");

    output.pop_back();
}

void BaseVRApp::ReadPSPControlSettings(const char *pspControlsIniFileName)
{
    char *control_key[2][6] = {
        {"Left", "Up", "Right", "Down", "L", "An.Up"},
        {"Square", "Triangle", "Circle", "Cross", "R", "An.Left" }
    };

    FILE *f = nullptr;
    fopen_s(&f, pspControlsIniFileName, "r");
    if (f == nullptr)
        return;

    char txt[256];
    while (!feof(f))
    {
        memset(&txt[0], 0, 256);
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
            for (int j = 0; j < 6; j++)
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
                                    //m_keys[i * 6 + j] = map_entry.first;
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

const wchar_t *BaseVRApp::GetSourceParentClassName() const
{
    return L"PPSSPPWnd";
}

const wchar_t *BaseVRApp::GetSourceParentWindowName() const
{
    return nullptr;
}

const wchar_t *BaseVRApp::GetSourceClassName() const
{
    return L"PPSSPPDisplay";
}
const wchar_t *BaseVRApp::GetSourceWindowName() const
{
    return nullptr;
}

void BaseVRApp::SetSourceParentHWnd(HWND hwnd)
{
    m_source_parent_window = hwnd;
}

void BaseVRApp::SetSourceHWnd(HWND hwnd)
{
    m_source_window = hwnd;
}

BOOL CALLBACK MyEnumWindowsProc(HWND   hwnd,LPARAM lParam)
{
    BOOL found = TRUE;

    std::pair<BaseVRApp *, bool> *data = (std::pair<BaseVRApp *, bool> *)lParam;
    const wchar_t *sclassName = data->second ? data->first->GetSourceParentClassName() : data->first->GetSourceClassName();
    const wchar_t *sWindowTitle = data->second ? data->first->GetSourceParentWindowName() : data->first->GetSourceWindowName();

    if (sWindowTitle)
    {
        wchar_t title[256];
        GetWindowText(hwnd, title, 256);
        found = (wcsstr(title, sWindowTitle) != nullptr) ? TRUE : FALSE;
    }

    if (sclassName)
    {
        wchar_t className[256];
        GetClassName(hwnd, className, 256);
        found = (wcsstr(className, sclassName) != nullptr) ? found : FALSE;
    }
    if (sclassName == nullptr && sWindowTitle == nullptr)
        return FALSE;

    if (found)
    {
        if (data->second)
            data->first->SetSourceParentHWnd(hwnd);
        else
            data->first->SetSourceHWnd(hwnd);
        return FALSE;
    }

    return TRUE;
}


bool BaseVRApp::GetPSPRect(RECT &rect)
{
    if (m_source_parent_window == 0)
    {
        m_source_parent_window = FindWindow(GetSourceParentClassName(), GetSourceParentWindowName());

        if (m_source_parent_window)
            SetWindowPos(m_source_parent_window, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    if (m_source_parent_window == 0)
    {
        std::pair<BaseVRApp *, bool> data(this, true);
        BOOL found = EnumWindows(MyEnumWindowsProc, (LPARAM)&data);

        if (!m_source_parent_window)
            SetWindowPos(m_source_parent_window, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    
    if (m_source_parent_window == 0)
        return false;

    m_source_window = FindWindowEx(m_source_parent_window, 0, GetSourceClassName(), GetSourceWindowName());

    if (m_source_window == 0)
    {
        std::pair<BaseVRApp *, bool> data(this, false);
        BOOL found = EnumChildWindows(m_source_parent_window, MyEnumWindowsProc, (LPARAM)&data);

        if (m_source_window == 0)
            return false;
    }

    if (m_source_window)
    {
        GetWindowRect(m_source_window, &rect);
        return true;
    }

    return false;
}
// this function initializes D3D and VR
bool BaseVRApp::init(HWND hWnd)
{
    UINT createDeviceFlags = 0;

    RECT PsPrect;
    if (!GetPSPRect(PsPrect))
    {
        OutputError(E_FAIL, __FILE__, __LINE__);
        return false;
    }

#ifdef DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

#ifndef VR_DISABLED

    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;

    m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

    if (eError != vr::VRInitError_None)
    {
        m_pHMD = NULL;
        char buf[1024];
        sprintf_s(buf, ARRAYSIZE(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        string temp(buf);
        wstring wtemp(temp.begin(), temp.end());
        MessageBox(hWnd, wtemp.c_str(), L"VR_Init Failed", 0);
        return false;
    }

    m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

    dprintf("width = %d, height = %d", m_nRenderWidth, m_nRenderHeight);


    m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
    if (!m_pRenderModels)
    {
        m_pHMD = NULL;
        vr::VR_Shutdown();

        char buf[1024];
        sprintf_s(buf, ARRAYSIZE(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        string temp(buf);
        wstring wtemp(temp.begin(), temp.end());
        MessageBox(hWnd, wtemp.c_str(), L"VR_Init Failed", NULL);
        return false;
    }

    if (!vr::VRCompositor())
    {
        dprintf("Compositor initialization failed. See log file for details\n");
        return false;
    }

#endif

    // CREATE DEVICE AND SWAP CHAIN
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE, // the first thing to try, if failed, go to the next
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0, // texture size and others..
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC swapDesc;
    ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = VRCLIENTWIDTH;
    swapDesc.BufferDesc.Height = VRCLIENTHEIGHT;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // unsigned normal
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hWnd;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Windowed = true;
    swapDesc.SampleDesc.Count = 1; // multisampling, which antialiasing for geometry. Turn it off
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // alt-enter fullscreen

    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    HRESULT errorCode;
    for (unsigned i = 0; i < numDriverTypes; ++i)
    {
        errorCode = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL, createDeviceFlags,
            featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice,
            &m_featureLevel, &m_pImmediateContext);

        if (SUCCEEDED(errorCode))
        {
            m_driverType = driverTypes[i];
            break;
        }
    }

    if (FAILED(errorCode))
    {
        OutputDebugString(_T("FAILED TO CREATE DEVICE AND SWAP CHAIN"));
        MyDebug(_T("FAILED TO CREATE DEVICE AND SWAP CHAIN"));
        return false;
    }

    CComPtr<ID3D11Texture2D> pBackBufferTex;
    HRESULT result;
    // CREATE RENDER TARGET VIEW
    result = m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBufferTex));
    if (FAILED(result))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC BBDesc;
    ZeroMemory(&BBDesc, sizeof(D3D11_TEXTURE2D_DESC));
    pBackBufferTex->GetDesc(&BBDesc);

    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
    ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    RTVDesc.Format = BBDesc.Format;
    RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    //RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    RTVDesc.Texture2D.MipSlice = 0;

    result = m_pDevice->CreateRenderTargetView(pBackBufferTex, &RTVDesc, &m_pRenderTargetView);
    if (FAILED(result))
    {
        MyDebug(_T("ERROR"));
    }

    // CREATE DEPTH STENCIL
    CComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = m_nRenderWidth;// swapDesc.BufferDesc.Width;
    descDepth.Height = m_nRenderHeight;// swapDesc.BufferDesc.Height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;// DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT;;//pDeviceSettings->d3d11.AutoDepthStencilFormat;
                                                        // DXGI_FORMAT_D32_FLOAT_S8X24_UINT
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    result = m_pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
    if (FAILED(result))
        return false;


    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    result = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);
    if (FAILED(result))
    {
        return false;
    }

    // Bind depth stencil state
    m_pImmediateContext->OMSetDepthStencilState(m_pDSState, 1);



    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;// DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    result = m_pDevice->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
        &descDSV, // Depth stencil desc
        &m_pDepthStencilView);  // [out] Depth stencil view

    if (FAILED(result))
    {
        WCHAR buf[100];
        wsprintf(buf, L"%x", result);
        MyDebug(buf);
        MyDebug(L"CreateDepthStencilView failed.");
        return false;
    }

    //BIND RENDER TARGET VIEW
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); // depth stencil view is for shadow map


    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    // Clear the second depth stencil state before setting the parameters.
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    result = m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    if (FAILED(result))
    {
        return false;
    }

    //VIEWPORT CREATION
    m_viewport.Width = static_cast<float>(m_nRenderWidth);
    m_viewport.Height = static_cast<float>(m_nRenderHeight);
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    // BIND VIEWPORT
    m_pImmediateContext->RSSetViewports(1, &m_viewport);


    m_DesktopGrabber.reset(new DesktopGrabber(m_pImmediateContext));
    HRESULT hr = m_DesktopGrabber->InitDupl(m_pDevice, PsPrect);
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return false;
    }

    ID3D11ShaderResourceView*ShaderResource = m_DesktopGrabber->GetDesktopResourceView();
    if (ShaderResource == nullptr)
    {
        return false;
    }

    m_source_texture.reset(new Texture());
    m_source_texture->SetTexture(ShaderResource);

    // Create the camera object.
    m_CameraLeft.reset(new Camera);
    if (!m_CameraLeft)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_CameraLeft->SetPosition(0.0f, 0.0f, -10.0f);

    // Create the camera object.
    m_CameraRight.reset(new Camera);
    if (!m_CameraRight)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_CameraRight->SetPosition(1.5f, 0.0f, -10.0f);

    if (!setupWorld())
        return false;

    if (NeedScreen())
    {
        m_Screen.reset(new Screen);

        int rect_width = PsPrect.right - PsPrect.left + 1;
        int rect_height = PsPrect.bottom - PsPrect.top + 1;

        float sx = (float)rect_width / (float)rect_height;

        // Initialize the model object.
        result = m_Screen->Initialize(m_pDevice, sx);
        if (!result)
        {
            MessageBox(hWnd, L"Could not initialize the model object.", L"Error", MB_OK);
            return false;
        }
    }

    // Create the color shader object.
    m_ColorShader.reset(new ColorShader);
    if (!m_ColorShader)
    {
        return false;
    }

    // Initialize the color shader object.
    result = m_ColorShader->Initialize(m_pDevice, hWnd);
    if (!result)
    {
        MessageBox(hWnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }


    // Create the render to texture object.
    m_RenderTextureLeft.reset(new RenderTexture);
    if (!m_RenderTextureLeft)
    {
        return false;
    }

    // Initialize the render to texture object.
    result = m_RenderTextureLeft->Initialize(m_pDevice, m_nRenderWidth, m_nRenderHeight);
    if (!result)
    {
        return false;
    }

    m_RenderTextureRight.reset(new RenderTexture);
    if (!m_RenderTextureRight)
    {
        return false;
    }

    // Initialize the render to texture object.
    result = m_RenderTextureRight->Initialize(m_pDevice, m_nRenderWidth, m_nRenderHeight);
    if (!result)
    {
        return false;
    }

    // Create the debug window object.
    m_DebugWindowLeft.reset(new DebugWindow);
    if (!m_DebugWindowLeft)
    {
        return false;
    }

    // Initialize the debug window object.
    result = m_DebugWindowLeft->Initialize(m_pDevice, VRCLIENTWIDTH, VRCLIENTHEIGHT, VRCLIENTWIDTH / 2, VRCLIENTHEIGHT);
    if (!result)
    {
        MessageBox(hWnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
        return false;
    }

    m_DebugWindowRight.reset(new DebugWindow);
    if (!m_DebugWindowRight)
    {
        return false;
    }
    // Initialize the debug window object.
    result = m_DebugWindowRight->Initialize(m_pDevice, VRCLIENTWIDTH, VRCLIENTHEIGHT, VRCLIENTWIDTH / 2, VRCLIENTHEIGHT);


    // Create an orthographic projection matrix for 2D rendering.
    //D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
    DirectX::XMMATRIX mo = DirectX::XMMatrixOrthographicLH((float)VRCLIENTWIDTH, (float)VRCLIENTHEIGHT, 0, 10);
    m_orthoMatrix.set((const float*)&mo.r);

    if ( NeedScreen())
        m_Screen->SetTexture(m_source_texture.get());

    m_fScale = 0.3f;
    m_fScaleSpacing = 4.0f;

    m_fNearClip = 0.1f;
    m_fFarClip = 30.0f;

    SetupCameras();

    if (!vr::VRCompositor())
    {
        printf("Compositor initialization failed. See log file for details\n");
        return false;
    }

    return true;

}


void BaseVRApp::TurnZBufferOn()
{
    m_pImmediateContext->OMSetDepthStencilState(m_pDSState, 1);
    return;
}


void BaseVRApp::TurnZBufferOff()
{
    m_pImmediateContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
    return;
}



bool BaseVRApp::RenderScene(vr::Hmd_Eye nEye)
{
    bool result;
    D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;


    projectionMatrix = GetCurrentViewProjectionMatrix(nEye);

    if (NeedScreen())
    {
        // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
        m_Screen->Render(m_pImmediateContext);
        // Render the model using the color shader.
        result = m_ColorShader->Render(m_pImmediateContext, m_Screen->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Screen->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
            MyDebug(_T("render failed"));
        }
    }

    return renderWorld(worldMatrix, viewMatrix, projectionMatrix, nEye == vr::Hmd_Eye::Eye_Left);
}

bool BaseVRApp::RenderToTexture()
{
    bool result;


    // Set the render target to be the render to texture.
    m_RenderTextureLeft->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);
    //Clear the render to texture background to blue so we can differentiate it from the rest of the normal scene.

    // Clear the render to texture.
    m_RenderTextureLeft->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, m_background_colors[0], m_background_colors[1], m_background_colors[2], 1.0f);

    // Render the scene now and it will draw to the render to texture instead of the back buffer.
    result = RenderScene(vr::Hmd_Eye::Eye_Left);
    if (!result)
    {
        return false;
    }


    // Set the render target to be the render to texture.
    m_RenderTextureRight->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);
    //Clear the render to texture background to blue so we can differentiate it from the rest of the normal scene.

    // Clear the render to texture.
    m_RenderTextureRight->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, m_background_colors[0], m_background_colors[1], m_background_colors[2], 1.0f);

    // Render the scene now and it will draw to the render to texture instead of the back buffer.
    result = RenderScene(vr::Hmd_Eye::Eye_Right);
    if (!result)
    {
        return false;
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    return true;
}

uint64_t BaseVRApp::ButtonsFromState(const vr::VRControllerState_t &state)
{
    uint64_t buttons = 0;
    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
    {
        if (state.rAxis[0].x > 0 && fabs(state.rAxis[0].x) > fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Right);
        }
        else if (state.rAxis[0].x < 0 && fabs(state.rAxis[0].x) > fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Left);
        }
        else if (state.rAxis[0].y > 0 && fabs(state.rAxis[0].x) < fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Up);
        }
        else if (state.rAxis[0].y < 0 && fabs(state.rAxis[0].x) < fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Down);
        }
    }

    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger))
    {
        if (state.rAxis[1].x > 0)
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_A);
        }
    }

    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu))
    {
        buttons |= vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu);
    }

    return buttons;
}

void BaseVRApp::FindKeyboard()
{
    UINT numDevices = 0;
    GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
    if (numDevices == 0)
        return;

    std::vector<RAWINPUTDEVICELIST> devices(numDevices);
    GetRawInputDeviceList(&devices[0], &numDevices, sizeof(RAWINPUTDEVICELIST));
    for (auto &device : devices)
    {
        if (device.dwType == RIM_TYPEKEYBOARD)
        {
            m_keyboard_handle = device.hDevice;
            return;
        }
    }
}

void BaseVRApp::SendKey(int action, int key)
{
    if (action == WM_KEYDOWN)
    {
        ::SendMessage(m_source_parent_window, WM_CHAR, key, 1);
    }
    else if (action == WM_KEYUP)
    {
        ::SendMessage(m_source_parent_window, WM_CHAR, key, (1L << 31));
    }
}

void BaseVRApp::ProcessButton(int , const vr::VRControllerState_t &)
{
}


// this is the function used to render a single frame
void BaseVRApp::render_frame(void)
{
    bool result;
    bool timeout = false;

    int controller_id = 0;
    for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
    {
        if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
            continue;

        if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
            continue;

        vr::VRControllerState_t state;
        if (m_pHMD->GetControllerState(unTrackedDevice, &state))
        {
            ProcessButton(controller_id, state);
        }

        controller_id++;
    }

    HandleController();

    RECT pspRect;
    if (!GetPSPRect(pspRect))
        return;

    HRESULT hr = m_DesktopGrabber->GetFrame(&timeout, pspRect);
    if (timeout)
        return;
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
    }
    // Render the entire scene to the texture first.
    result = RenderToTexture();
    if (!result)
    {
        return;
    }

    D3DXMATRIX orthoMatrix;

    TurnZBufferOff();

    result = m_DebugWindowLeft->Render(m_pImmediateContext, 0, 0);
    if (!result)
    {
        return;
    }

    orthoMatrix = m_orthoMatrix;

    Matrix4 identity;
    // Render the debug window using the texture shader.
    result = m_ColorShader->Render(m_pImmediateContext, m_DebugWindowLeft->GetIndexCount(), identity, identity,
        orthoMatrix, m_RenderTextureLeft->GetShaderResourceView());
    if (!result)
    {
        return;
    }


    result = m_DebugWindowRight->Render(m_pImmediateContext, 546, 0);
    if (!result)
    {
        return;
    }

    orthoMatrix = m_orthoMatrix;

    // Render the debug window using the texture shader.
    result = m_ColorShader->Render(m_pImmediateContext, m_DebugWindowRight->GetIndexCount(), identity, identity,
        orthoMatrix, m_RenderTextureRight->GetShaderResourceView());
    if (!result)
    {
        return;
    }

    TurnZBufferOn();


    m_pSwapChain->Present(0, 0);

    vr::Texture_t leftEyeTexture = { m_RenderTextureLeft->GetTexture(), vr::API_DirectX, vr::ColorSpace_Auto };
    vr::EVRCompositorError error1 = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    vr::Texture_t rightEyeTexture = { m_RenderTextureRight->GetTexture(), vr::API_DirectX, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    if (error1)
        dprintf("error is %d \n", error1);

    UpdateHMDMatrixPose();

}



std::string BaseVRApp::MatrixToString(const Matrix4& matrix)
{
    char buf[1000];
    int start = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            start += sprintf_s(buf + start, ARRAYSIZE(buf) - start, "%.5f, ", matrix[j * 4 + i]);
        }
        start += sprintf_s(buf + start, ARRAYSIZE(buf) - start, "\n");
    }

    string temp(buf);
    return temp;
}


Matrix4 BaseVRApp::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return Matrix4();

    vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
    Matrix4 matrixObj(
        matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
        matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
        matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
        matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
    );

    return matrixObj.invert();
}

Matrix4 BaseVRApp::GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return Matrix4();

    vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, m_fNearClip, m_fFarClip, vr::API_DirectX);

    return Matrix4(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

void BaseVRApp::SetupCameras()
{
    m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
    m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
    m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
    m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}


Matrix4 BaseVRApp::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
    Matrix4 matMVP;
    if (nEye == vr::Eye_Left)
    {
        matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose;
    }
    else if (nEye == vr::Eye_Right)
    {
        matMVP = m_mat4ProjectionRight * m_mat4eyePosRight *  m_mat4HMDPose;
    }

    return matMVP;
}


Matrix4 BaseVRApp::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
    Matrix4 matrixObj(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrixObj;
}

void BaseVRApp::UpdateHMDMatrixPose()
{
    if (!m_pHMD)
        return;

    vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    m_iValidPoseCount = 0;
    m_strPoseClasses = "";
    for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
    {
        if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
        {
            m_iValidPoseCount++;
            m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
            if (m_rDevClassChar[nDevice] == 0)
            {
                switch (m_pHMD->GetTrackedDeviceClass(nDevice))
                {
                case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
                case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
                case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
                case vr::TrackedDeviceClass_Other:             m_rDevClassChar[nDevice] = 'O'; break;
                case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
                default:                                       m_rDevClassChar[nDevice] = '?'; break;
                }
            }
            m_strPoseClasses += m_rDevClassChar[nDevice];
        }
    }

    if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd].invert();
    }
    else
    {
        dprintf("pose not valid\n");
    }
}


// this is the function that cleans up D3D and VR
void BaseVRApp::clean(void)
{
    // Release the color shader object.
    if (m_ColorShader.get())
        m_ColorShader.release();

    // Release the model object.	
    if (m_Screen.get())
        m_Screen.release();

    // Release the camera object.
    if (m_CameraLeft.get())
        m_CameraLeft.release();

    if (m_pImmediateContext) m_pImmediateContext->ClearState();
    m_pDepthStencilView.Release();
    m_pDepthStencilView.Release();
    m_pRenderTargetView.Release();
    m_pSwapChain.Release();
    m_pImmediateContext.Release();
    m_pDevice.Release();

    if (m_DesktopGrabber.get())
        m_DesktopGrabber.release();

    if (m_RenderTextureLeft.get())
        m_RenderTextureLeft->Shutdown();
    if (m_RenderTextureRight.get())
        m_RenderTextureRight->Shutdown();
    if (m_DebugWindowLeft.get())
        m_DebugWindowLeft->Shutdown();
    if (m_DebugWindowRight.get())
        m_DebugWindowRight->Shutdown();

    if (m_pHMD)
    {
        vr::VR_Shutdown();
    }
}

bool BaseVRApp::NeedScreen()
{
    return true;
}