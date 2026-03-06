
struct mapline {
    char key[21];
    uint8_t usb_hid_key;
};

const uint8_t lines = 165;

struct mapline mapusb[lines] =
{
{ "KEY_A", 0x04 }, // Keyboard a and A OK
{ "KEY_B", 0x05 }, // Keyboard b and B
{ "KEY_C", 0x06 }, // Keyboard c and C
{ "KEY_D", 0x07 }, // Keyboard d and D
{ "KEY_E", 0x08 }, // Keyboard e and E
{ "KEY_F", 0x09 }, // Keyboard f and F
{ "KEY_G", 0x0a }, // Keyboard g and G
{ "KEY_H", 0x0b }, // Keyboard h and H
{ "KEY_I", 0x0c }, // Keyboard i and I
{ "KEY_J", 0x0d }, // Keyboard j and J
{ "KEY_K", 0x0e }, // Keyboard k and K
{ "KEY_L", 0x0f }, // Keyboard l and L
{ "KEY_M", 0x10 }, // Keyboard m and M
{ "KEY_N", 0x11 }, // Keyboard n and N
{ "KEY_O", 0x12 }, // Keyboard o and O
{ "KEY_P", 0x13 }, // Keyboard p and P
{ "KEY_Q", 0x14 }, // Keyboard q and Q
{ "KEY_R", 0x15 }, // Keyboard r and R
{ "KEY_S", 0x16 }, // Keyboard s and S
{ "KEY_T", 0x17 }, // Keyboard t and T
{ "KEY_U", 0x18 }, // Keyboard u and U
{ "KEY_V", 0x19 }, // Keyboard v and V
{ "KEY_W", 0x1a }, // Keyboard w and W
{ "KEY_X", 0x1b }, // Keyboard x and X
{ "KEY_Y", 0x1c }, // Keyboard y and Y
{ "KEY_Z", 0x1d }, // Keyboard z and Z OK
{ "KEY_1", 0x1e }, // Keyboard 1 and ! OK
{ "KEY_2", 0x1f }, // Keyboard 2 and @ OK
{ "KEY_3", 0x20 }, // Keyboard 3 and # OK
{ "KEY_4", 0x21 }, // Keyboard 4 and $ OK
{ "KEY_5", 0x22 }, // Keyboard 5 and % OK
{ "KEY_6", 0x23 }, // Keyboard 6 and ^ OK
{ "KEY_7", 0x24 }, // Keyboard 7 and & OK
{ "KEY_8", 0x25 }, // Keyboard 8 and * OK
{ "KEY_9", 0x26 }, // Keyboard 9 and ( OK
{ "KEY_0", 0x27 }, // Keyboard 0 and ) OK
{ "KEY_ENTER", 0x28 }, // Keyboard Return (ENTER) OK
{ "KEY_ESC", 0x29 }, // Keyboard ESCAPE OK
{ "KEY_BACKSPACE", 0x2a }, // Keyboard DELETE (Backspace) OK
{ "KEY_TAB", 0x2b }, // Keyboard Tab OK
{ "KEY_SPACE", 0x2c }, // Keyboard Spacebar OK
{ "KEY_MINUS", 0x2d }, // Keyboard - and _ OK
{ "KEY_EQUAL", 0x2e }, // Keyboard = and + OK
{ "KEY_LEFTBRACE", 0x2f }, // Keyboard [ and { OK
{ "KEY_RIGHTBRACE", 0x30 }, // Keyboard ] and } OK
{ "KEY_BACKSLASH", 0x31 }, // Keyboard \ and | OK
//{ "KEY_NONUSHASH", 0x32 }, // Keyboard Non-US # and ~ KEY_BACKSLASH dupl
{ "KEY_SEMICOLON", 0x33 }, // Keyboard ; and : OK
{ "KEY_APOSTROPHE", 0x34 }, // Keyboard ' and " OK
{ "KEY_GRAVE", 0x35 }, // Keyboard ` and ~ OK
{ "KEY_COMMA", 0x36 }, // Keyboard , and < OK
{ "KEY_DOT", 0x37 }, // Keyboard . and > OK
{ "KEY_SLASH", 0x38 }, // Keyboard / and ? OK
{ "KEY_CAPSLOCK", 0x39 }, // Keyboard Caps Lock OK
{ "KEY_F1", 0x3a }, // Keyboard F1 OK
{ "KEY_F2", 0x3b }, // Keyboard F2
{ "KEY_F3", 0x3c }, // Keyboard F3
{ "KEY_F4", 0x3d }, // Keyboard F4
{ "KEY_F5", 0x3e }, // Keyboard F5
{ "KEY_F6", 0x3f }, // Keyboard F6
{ "KEY_F7", 0x40 }, // Keyboard F7
{ "KEY_F8", 0x41 }, // Keyboard F8
{ "KEY_F9", 0x42 }, // Keyboard F9
{ "KEY_F10", 0x43 }, // Keyboard F10 OK
{ "KEY_F11", 0x44 }, // Keyboard F11 OK
{ "KEY_F12", 0x45 }, // Keyboard F12 OK
{ "KEY_SYSRQ", 0x46 }, // Keyboard Print Screen OK
{ "KEY_SCROLLLOCK", 0x47 }, // Keyboard Scroll Lock OK
{ "KEY_PAUSE", 0x48 }, // Keyboard Pause OK
{ "KEY_INSERT", 0x49 }, // Keyboard Insert OK
{ "KEY_HOME", 0x4a }, // Keyboard Home OK
{ "KEY_PAGEUP", 0x4b }, // Keyboard Page Up OK
{ "KEY_DELETE", 0x4c }, // Keyboard Delete Forward OK
{ "KEY_END", 0x4d }, // Keyboard End OK
{ "KEY_PAGEDOWN", 0x4e }, // Keyboard Page Down OK
{ "KEY_RIGHT", 0x4f }, // Keyboard Right Arrow OK
{ "KEY_LEFT", 0x50 }, // Keyboard Left Arrow OK
{ "KEY_DOWN", 0x51 }, // Keyboard Down Arrow OK
{ "KEY_UP", 0x52 }, // Keyboard Up Arrow OK
{ "KEY_NUMLOCK", 0x53 }, // Keyboard Num Lock and Clear OK
{ "KEY_KPSLASH", 0x54 }, // Keypad / OK
{ "KEY_KPASTERISK", 0x55 }, // Keypad * OK
{ "KEY_KPMINUS", 0x56 }, // Keypad - OK
{ "KEY_KPPLUS", 0x57 }, // Keypad + OK
{ "KEY_KPENTER", 0x58 }, // Keypad ENTER OK
{ "KEY_KP1", 0x59 }, // Keypad 1 and End OK
{ "KEY_KP2", 0x5a }, // Keypad 2 and Down Arrow OK
{ "KEY_KP3", 0x5b }, // Keypad 3 and PageDn OK
{ "KEY_KP4", 0x5c }, // Keypad 4 and Left Arrow OK
{ "KEY_KP5", 0x5d }, // Keypad 5 OK
{ "KEY_KP6", 0x5e }, // Keypad 6 and Right Arrow OK
{ "KEY_KP7", 0x5f }, // Keypad 7 and Home OK
{ "KEY_KP8", 0x60 }, // Keypad 8 and Up Arrow OK
{ "KEY_KP9", 0x61 }, // Keypad 9 and Page Up OK
{ "KEY_KP0", 0x62 }, // Keypad 0 and Insert
{ "KEY_KPDOT", 0x63 }, // Keypad . and Delete OK
{ "KEY_102ND", 0x64 }, // Keyboard Non-US \ and | OK
{ "KEY_COMPOSE", 0x65 }, // Keyboard Application OK
{ "KEY_POWER", 0x66 }, // Keyboard Power OK
{ "KEY_KPEQUAL", 0x67 }, // Keypad = OK
{ "KEY_F13", 0x68 }, // Keyboard F13 OK
{ "KEY_F14", 0x69 }, // Keyboard F14
{ "KEY_F15", 0x6a }, // Keyboard F15
{ "KEY_F16", 0x6b }, // Keyboard F16
{ "KEY_F17", 0x6c }, // Keyboard F17
{ "KEY_F18", 0x6d }, // Keyboard F18
{ "KEY_F19", 0x6e }, // Keyboard F19
{ "KEY_F20", 0x6f }, // Keyboard F20
{ "KEY_F21", 0x70 }, // Keyboard F21
{ "KEY_F22", 0x71 }, // Keyboard F22
{ "KEY_F23", 0x72 }, // Keyboard F23
{ "KEY_F24", 0x73 }, // Keyboard F24 OK
{ "KEY_OPEN", 0x74 }, // Keyboard Execute OK
{ "KEY_HELP", 0x75 }, // Keyboard Help OK
{ "KEY_PROPS", 0x76 }, // Keyboard Menu OK
{ "KEY_FRONT", 0x77 }, // Keyboard Select OK
{ "KEY_STOP", 0x78 }, // Keyboard Stop OK
{ "KEY_AGAIN", 0x79 }, // Keyboard Again OK
{ "KEY_UNDO", 0x7a }, // Keyboard Undo OK
{ "KEY_CUT", 0x7b }, // Keyboard Cut OK
{ "KEY_COPY", 0x7c }, // Keyboard Copy OK
{ "KEY_PASTE", 0x7d }, // Keyboard Paste OK
{ "KEY_FIND", 0x7e }, // Keyboard Find OK
{ "KEY_MUTE", 0x7f }, // Keyboard Mute OK
{ "KEY_VOLUMEUP", 0x80 }, // Keyboard Volume Up OK
{ "KEY_VOLUMEDOWN", 0x81 }, // Keyboard Volume Down OK
//{ "KEY_CAPSLOCK", 0x82 }, // Keyboard Locking Caps Lock unk
//{ "KEY_NUMLOCK", 0x83 },  // Keyboard Locking Num Lock unk
//{ "KEY_SCROLLLOCK", 0x84 },  // Keyboard Locking Scroll Lock unk
{ "KEY_KPCOMMA", 0x85 }, // Keypad Comma OK
// Keypad Equal Sign 0x86 unk
{ "KEY_RO", 0x87 }, // Keyboard International1 OK
{ "KEY_KATAKANAHIRAGANA", 0x88 }, // Keyboard International2 OK
{ "KEY_YEN", 0x89 }, // Keyboard International3 OK
{ "KEY_HENKAN", 0x8a }, // Keyboard International4 OK
{ "KEY_MUHENKAN", 0x8b }, // Keyboard International5 OK
{ "KEY_KPJPCOMMA", 0x8c }, // Keyboard International6 OK
//  0x8d Keyboard International7 unk
//  0x8e Keyboard International8 unk
//  0x8f Keyboard International9 unk
{ "KEY_HANGEUL", 0x90 }, // Keyboard LANG1 OK
{ "KEY_HANJA", 0x91 }, // Keyboard LANG2 OK
{ "KEY_KATAKANA", 0x92 }, // Keyboard LANG3 OK
{ "KEY_HIRAGANA", 0x93 }, // Keyboard LANG4 OK
{ "KEY_ZENKAKUHANKAKU", 0x94 }, // Keyboard LANG5 OK
//{ "KEY_LANG6", 0x95 }, // Keyboard LANG6 unk
//{ "KEY_LANG7", 0x96 }, // Keyboard LANG7 unk
//{ "KEY_LANG8", 0x97 }, // Keyboard LANG8 unk
// KEY_LANG9 0x98  Keyboard LANG9 unk
//{ "KEY_ALTERASE",  0x99},  // Keyboard alternate erase unk
//{ "KEY_SYSREQATTN",  0x9A},  // Keyboard SysReq/Attention unk
//{ "KEY_CANCEL",      0x9B},  // Keyboard Cancel unk
//{ "KEY_CLEAR",       0x9C},  // Keyboard Clear KEY_DELETE dupl
//{ "KEY_PRIOR",       0x9D},  // Keyboard Prior unk
//{ "KEY_RETURN",      0x9E},  // Keyboard Return
//{ "KEY_SEPARATOR",   0x9F},  // Keyboard Separator
//{ "KEY_OUT",         0xA0},  // Keyboard Out
//{ "KEY_OPER",        0xA1},  // Keyboard Oper
//{ "KEY_CLRAGAIN",    0xA2},  // Keyboard Clear/Again
//{ "KEY_CRSEL",       0xA3},  // Keyboard CrSel/Props ?
//{ "KEY_EXSEL",       0xA4},  // Keyboard ExSel ?
//{ "KEY_KP00",        0xB0},  // Keypad 00
//{ "KEY_KP000",       0xB1},  // Keypad 000
//{ "KEY_THOUSANDSEP", 0xB2},  // Thousands separator, depends on Locale either . or ,
//{ "KEY_DECIMALSEP",  0xB3},  // Decimal separator, depends on Locale, either , or .
//{ "KEY_CURRENCY",    0xB4},  // Currency Unit, depends on locale e.g. $
//{ "KEY_CURRENCYSUB", 0xB5},  // Currency sub-unit, depends on locale e.g. ¢ unk
{ "KEY_KPLEFTPAREN", 0xb6 }, // Keypad ( OK
{ "KEY_KPRIGHTPAREN", 0xb7 }, // Keypad ) OK
//{ "KEY_KPLCURLY",    0xB8},  // Keypad { unk
//{ "KEY_KPRCURLY",    0xB9},  // Keypad },
//{ "KEY_KPTAB",       0xBA},  // Keypad Tab
//{ "KEY_KPBACKSPACE", 0xBB},  // Keypad Backspace
//{ "KEY_KPA",         0xBC},  // Keypad A
//{ "KEY_KPB",         0xBD},  // Keypad B
//{ "KEY_KPC",         0xBE},  // Keypad C
//{ "KEY_KPD",         0xBF},  // Keypad D
//{ "KEY_KPE",         0xC0},  // Keypad E
//{ "KEY_KPF",         0xC1},  // Keypad F
//{ "KEY_KPXOR",       0xC2},  // Keypad XOR
//{ "KEY_KPHAT",       0xC3},  // Keypad ^
//{ "KEY_KPPERCENT",   0xC4},  // Keypad %
//{ "KEY_KPLT",        0xC5},  // Keypad <
//{ "KEY_KPGT",        0xC6},  // Keypad >
//{ "KEY_KPAMP",       0xC7},  // Keypad &
//{ "KEY_KPAMPAMP",    0xC8},  // Keypad &&
//{ "KEY_KPBAR",       0xC9},  // Keypad |
//{ "KEY_KPBARBAR",    0xCA},  // Keypad ||
//{ "KEY_KPCOLON",     0xCB},  // Keypad :
//{ "KEY_KPHASH",      0xCC},  // Keypad #
//{ "KEY_KPSPACE",     0xCD},  // Keypad Space
//{ "KEY_KPAT",        0xCE},  // Keypad @
//{ "KEY_KPEXCLAM",    0xCF},  // Keypad !
//{ "KEY_KPMEMSTORE",  0xD0},  // Keypad Memory Store
//{ "KEY_KPMEMRECALL", 0xD1},  // Keypad Memory Recall
//{ "KEY_KPMEMCLEAR",  0xD2},  // Keypad Memory Clear
//{ "KEY_KPMEMADD",    0xD3},  // Keypad memory add
//{ "KEY_KPMEMSUB",    0xD4},  // Keypad Memory Subtract
//{ "KEY_KPMEMMULT",   0xD5},  // Keypad Memory Multiply
//{ "KEY_KPMEMDIV",    0xD6},  // Keypad Memory Divide
//{ "KEY_KPPLUSMINUS",  0xD7},  // Keypad +/- unk
//{ "KEY_KPCLEAR",     0xD8}, // Keypad Clear KEY_DELETE dupl
//{ "KEY_KPCLEARENT",  0xD9},  // Keypad Clear Entry unk
//{ "KEY_KPBINARY",    0xDA},  // Keypad Binary
//{ "KEY_KPOCTAL",     0xDB},  // keypad Octal
//{ "KEY_KPDECIMAL",   0xDC},  // Keypad Decimal
//{ "KEY_KPHEX",       0xDD},  // Keypad Hexadecimal unk
// 0xDE // reserved
// 0xDF // reserved unk
{ "KEY_LEFTCTRL", 0xe0 }, // Keyboard Left Control OK
{ "KEY_LEFTSHIFT", 0xe1 }, // Keyboard Left Shift OK
{ "KEY_LEFTALT", 0xe2 }, // Keyboard Left Alt OK
{ "KEY_LEFTMETA", 0xe3 }, // Keyboard Left GUI OK
{ "KEY_RIGHTCTRL", 0xe4 }, // Keyboard Right Control OK
{ "KEY_RIGHTSHIFT", 0xe5 }, // Keyboard Right Shift OK
{ "KEY_RIGHTALT", 0xe6 }, // Keyboard Right Alt OK
{ "KEY_RIGHTMETA", 0xe7 }, // Keyboard Right GUI OK
{ "KEY_PLAYPAUSE",	0xE8},  // Reserved? ...\/ OK
{ "KEY_STOPCD",	0xE9}, // OK
{ "KEY_PREVIOUSSONG",  0xEA}, // OK
{ "KEY_NEXTSONG",	0xEB}, // OK
{ "KEY_EJECTCD",	0xEC}, // OK
//KEY_VOLUMEUP  0xed dupl
//KEY_VOLUMEDOWN 0xee dupl
//KEY_MUTE 0xef dupl
{ "KEY_WWW",	        0xF0}, // OK
{ "KEY_BACK",	0xF1}, // OK
{ "KEY_FORWARD",	0xF2}, // OK
// KEY_STOP 0xf3 dupl
// KEY_FIND 0xf4 dupl
{ "KEY_SCROLLUP",	0xF5}, // OK
{ "KEY_SCROLLDOWN",	0xF6}, // OK
{ "KEY_EDIT",	0xF7}, // OK
{ "KEY_SLEEP",	0xF8}, // OK
{ "KEY_SCREENLOCK",	0xF9}, // KEY_COFFEE -> KEY_SCREENLOCK OK
{ "KEY_REFRESH",	0xFA}, // OK
{ "KEY_CALC",	0xFB},  // OK
{ "ff",	0x00},  // ff stands for no key and that's what 0x00 does
{ "ff",	0xFF},  // if eeprom is empty, we receive 0xFF, and that means no key
};

struct mapline modifier[10] =
{
{ "KEY_LEFTCTRL",   0x01 }, // Keyboard Left Control
{ "KEY_LEFTSHIFT",  0x02 }, // Keyboard Left Shift
{ "KEY_LEFTALT",    0x04 }, // Keyboard Left Alt
{ "KEY_LEFTMETA",   0x08 }, // Keyboard Left GUI
{ "KEY_RIGHTCTRL",  0x10 }, // Keyboard Right Control
{ "KEY_RIGHTSHIFT", 0x20 }, // Keyboard Right Shift
{ "KEY_RIGHTALT",   0x40 }, // Keyboard Right Alt
{ "KEY_RIGHTMETA",  0x80 }, // Keyboard Right GUI
{ "ff",             0x00},  // ff stands for no key and that's what 0x00 does
{ "ff",             0xFF},  // if eeprom is empty, we receive 0xFF, and that means no key
};