#include <iostream>
#include <stdio.h>     
#include <stdlib.h>   
#include <Windows.h>
#include <time.h>
#include <fstream>
#include <vector>
#include <fcntl.h>
#include <string>
#include <optional>
#include <conio.h> 
#include <limits>
#include <random>
#include <sstream>
#include <type_traits>
#include <io.h>
#include <mutex>
#include <thread>
#include <algorithm>
#include <functional>
#include <locale>
#include <chrono>
#include <codecvt>
#include <filesystem>
#include <memory>
#include <map>
#include <print>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <xmllite.h>
#include <shlwapi.h>   // pour SHCreateStreamOnFile
#include <combaseapi.h>

#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "ole32.lib")  // pour COM
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winmm.lib")

// Alias pour raccourcir les types
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Nanoseconds = std::chrono::nanoseconds;

// ───────────── LETTRES ─────────────
#define KEY_A 0x41
#define KEY_B 0x42
#define KEY_C 0x43
#define KEY_D 0x44
#define KEY_E 0x45
#define KEY_F 0x46
#define KEY_G 0x47
#define KEY_H 0x48
#define KEY_I 0x49
#define KEY_J 0x4A
#define KEY_K 0x4B
#define KEY_L 0x4C
#define KEY_M 0x4D
#define KEY_N 0x4E
#define KEY_O 0x4F
#define KEY_P 0x50
#define KEY_Q 0x51
#define KEY_R 0x52
#define KEY_S 0x53
#define KEY_T 0x54
#define KEY_U 0x55
#define KEY_V 0x56
#define KEY_W 0x57
#define KEY_X 0x58
#define KEY_Y 0x59
#define KEY_Z 0x5A

// ───────────── CHIFFRES ─────────────
#define KEY_0 0x30
#define KEY_1 0x31
#define KEY_2 0x32
#define KEY_3 0x33
#define KEY_4 0x34
#define KEY_5 0x35
#define KEY_6 0x36
#define KEY_7 0x37
#define KEY_8 0x38
#define KEY_9 0x39

// ───────────── TOUCHES DE CONTRÔLE ─────────────
#define KEY_ENTER        VK_RETURN
#define KEY_BACKSPACE    VK_BACK
#define KEY_TAB          VK_TAB
#define KEY_SHIFT        VK_SHIFT
#define KEY_LSHIFT       VK_LSHIFT
#define KEY_RSHIFT       VK_RSHIFT
#define KEY_CONTROL      VK_CONTROL
#define KEY_LCONTROL     VK_LCONTROL
#define KEY_RCONTROL     VK_RCONTROL
#define KEY_ALT          VK_MENU
#define KEY_LALT         VK_LMENU
#define KEY_RALT         VK_RMENU
#define KEY_CAPSLOCK     VK_CAPITAL
#define KEY_ESC          VK_ESCAPE
#define KEY_SPACE        VK_SPACE

// ───────────── TOUCHES DE DÉPLACEMENT ─────────────
#define KEY_LEFT_ARROW   VK_LEFT
#define KEY_RIGHT_ARROW  VK_RIGHT
#define KEY_UP_ARROW     VK_UP
#define KEY_DOWN_ARROW   VK_DOWN
#define KEY_HOME         VK_HOME
#define KEY_END          VK_END
#define KEY_INSERT       VK_INSERT
#define KEY_DELETE       VK_DELETE
#define KEY_PAGE_UP      VK_PRIOR
#define KEY_PAGE_DOWN    VK_NEXT

// ───────────── CLAVIER NUMÉRIQUE (NUMPAD) ─────────────
#define KEY_NUMPAD0      VK_NUMPAD0
#define KEY_NUMPAD1      VK_NUMPAD1
#define KEY_NUMPAD2      VK_NUMPAD2
#define KEY_NUMPAD3      VK_NUMPAD3
#define KEY_NUMPAD4      VK_NUMPAD4
#define KEY_NUMPAD5      VK_NUMPAD5
#define KEY_NUMPAD6      VK_NUMPAD6
#define KEY_NUMPAD7      VK_NUMPAD7
#define KEY_NUMPAD8      VK_NUMPAD8
#define KEY_NUMPAD9      VK_NUMPAD9
#define KEY_DECIMAL      VK_DECIMAL
#define KEY_ADD          VK_ADD
#define KEY_SUBTRACT     VK_SUBTRACT
#define KEY_MULTIPLY     VK_MULTIPLY
#define KEY_DIVIDE       VK_DIVIDE

// ───────────── TOUCHES FONCTIONS (F1 → F24) ─────────────
#define KEY_F1           VK_F1
#define KEY_F2           VK_F2
#define KEY_F3           VK_F3
#define KEY_F4           VK_F4
#define KEY_F5           VK_F5
#define KEY_F6           VK_F6
#define KEY_F7           VK_F7
#define KEY_F8           VK_F8
#define KEY_F9           VK_F9
#define KEY_F10          VK_F10
#define KEY_F11          VK_F11
#define KEY_F12          VK_F12
#define KEY_F13          VK_F13
#define KEY_F14          VK_F14
#define KEY_F15          VK_F15
#define KEY_F16          VK_F16
#define KEY_F17          VK_F17
#define KEY_F18          VK_F18
#define KEY_F19          VK_F19
#define KEY_F20          VK_F20
#define KEY_F21          VK_F21
#define KEY_F22          VK_F22
#define KEY_F23          VK_F23
#define KEY_F24          VK_F24

// ───────────── SYMBOLES (selon la disposition clavier) ─────────────
#define KEY_SEMICOLON     VK_OEM_1       // ; :
#define KEY_PLUS          VK_OEM_PLUS    // + =
#define KEY_COMMA         VK_OEM_COMMA   // ,
#define KEY_MINUS         VK_OEM_MINUS   // - _
#define KEY_PERIOD        VK_OEM_PERIOD  // . >
#define KEY_SLASH         VK_OEM_2       // / ?
#define KEY_TILDE         VK_OEM_3       // ~ `
#define KEY_LEFT_BRACKET  VK_OEM_4       // [ {
#define KEY_BACKSLASH     VK_OEM_5       // \ |
#define KEY_RIGHT_BRACKET VK_OEM_6       // ] }
#define KEY_APOSTROPHE    VK_OEM_7       // ' "
#define KEY_OEM_8         VK_OEM_8       // Clavier étendu
#define KEY_OEM_102       VK_OEM_102     // "<" ou ">" selon le clavier (surtout européen)

// ───────────── TOUCHES SYSTÈME ─────────────
#define KEY_PAUSE         VK_PAUSE
#define KEY_PRINTSCREEN   VK_SNAPSHOT
#define KEY_NUMLOCK       VK_NUMLOCK
#define KEY_SCROLLLOCK    VK_SCROLL
#define KEY_WINDOWS_LEFT  VK_LWIN
#define KEY_WINDOWS_RIGHT VK_RWIN
#define KEY_APPS          VK_APPS        // Menu contextuel

// ───────────── TOUCHES MULTIMÉDIAS ─────────────
#define KEY_VOLUME_MUTE     VK_VOLUME_MUTE
#define KEY_VOLUME_DOWN     VK_VOLUME_DOWN
#define KEY_VOLUME_UP       VK_VOLUME_UP
#define KEY_MEDIA_NEXT      VK_MEDIA_NEXT_TRACK
#define KEY_MEDIA_PREV      VK_MEDIA_PREV_TRACK
#define KEY_MEDIA_STOP      VK_MEDIA_STOP
#define KEY_MEDIA_PLAY_PAUSE VK_MEDIA_PLAY_PAUSE
#define KEY_BROWSER_BACK     VK_BROWSER_BACK
#define KEY_BROWSER_FORWARD  VK_BROWSER_FORWARD
#define KEY_BROWSER_REFRESH  VK_BROWSER_REFRESH
#define KEY_BROWSER_STOP     VK_BROWSER_STOP
#define KEY_BROWSER_SEARCH   VK_BROWSER_SEARCH
#define KEY_BROWSER_FAVORITES VK_BROWSER_FAVORITES
#define KEY_BROWSER_HOME     VK_BROWSER_HOME
#define KEY_LAUNCH_MAIL      VK_LAUNCH_MAIL
#define KEY_LAUNCH_MEDIA     VK_LAUNCH_MEDIA_SELECT
#define KEY_LAUNCH_APP1      VK_LAUNCH_APP1
#define KEY_LAUNCH_APP2      VK_LAUNCH_APP2
