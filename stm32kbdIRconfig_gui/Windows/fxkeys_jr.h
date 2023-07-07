
#ifndef FXKEYS_H
#define FXKEYS_H

namespace FX {

struct fxkey_line{
	char key[32];
	uint32_t fx_key;
};

uint16_t fxkey_lines = 124;

struct fxkey_line fxkey_map[]
{
  {"KEY_A"                             , 0x0061},
  {"KEY_B"                             , 0x0062},
  {"KEY_C"                             , 0x0063},
  {"KEY_D"                             , 0x0064},
  {"KEY_E"                             , 0x0065},
  {"KEY_F"                             , 0x0066},
  {"KEY_G"                             , 0x0067},
  {"KEY_H"                             , 0x0068},
  {"KEY_I"                             , 0x0069},
  {"KEY_J"                             , 0x006A},
  {"KEY_K"                             , 0x006B},
  {"KEY_L"                             , 0x006C},
  {"KEY_M"                             , 0x006D},
  {"KEY_N"                             , 0x006E},
  {"KEY_O"                             , 0x006F},
  {"KEY_P"                             , 0x0070},
  {"KEY_Q"                             , 0x0071},
  {"KEY_R"                             , 0x0072},
  {"KEY_S"                             , 0x0073},
  {"KEY_T"                             , 0x0074},
  {"KEY_U"                             , 0x0075},
  {"KEY_V"                             , 0x0076},
  {"KEY_W"                             , 0x0077},
  {"KEY_X"                             , 0x0078},
  {"KEY_Y"                             , 0x0079},
  {"KEY_Z"                             , 0x007A},
  {"KEY_1"                             , 0x0031},
  {"KEY_2"                             , 0x0032},
  {"KEY_3"                             , 0x0033},
  {"KEY_4"                             , 0x0034},
  {"KEY_5"                             , 0x0035},
  {"KEY_6"                             , 0x0036},
  {"KEY_7"                             , 0x0037},
  {"KEY_8"                             , 0x0038},
  {"KEY_9"                             , 0x0039},
  {"KEY_0"                             , 0x0030},
  {"KEY_ENTER"                         , 0xFF0D},
  {"KEY_ESC"                           , 0xFF1B},
  {"KEY_BACKSPACE"                     , 0xFF08},
  {"KEY_TAB"                           , 0xFF09},
  {"KEY_SPACE"                         , 0x0020},
  {"KEY_MINUS"                         , 0x002D},
  {"KEY_EQUAL"                         , 0x003D},
  {"KEY_LEFTBRACE"                     , 0x007B},
  {"KEY_RIGHTBRACE"                    , 0x007D},
  {"KEY_BACKSLASH"                     , 0x005C},

  {"KEY_SEMICOLON"                     , 0x003B},
  {"KEY_APOSTROPHE"                    , 0x0027},
  {"KEY_GRAVE"                         , 0x0060},
  {"KEY_COMMA"                         , 0x002C},
  {"KEY_DOT"                           , 0x002E},
  {"KEY_SLASH"                         , 0x002F},
  {"KEY_CAPSLOCK"                      , 0xFFE5},
  {"KEY_F1"                            , 0xFFBE},
  {"KEY_F2"                            , 0xFFBF},
  {"KEY_F3"                            , 0xFFC0},
  {"KEY_F4"                            , 0xFFC1},
  {"KEY_F5"                            , 0xFFC2},
  {"KEY_F6"                            , 0xFFC3},
  {"KEY_F7"                            , 0xFFC4},
  {"KEY_F8"                            , 0xFFC5},
  {"KEY_F9"                            , 0xFFC6},
  {"KEY_F10"                           , 0xFFC7},
  {"KEY_F11"                           , 0xFFC8},
  {"KEY_F12"                           , 0xFFC9},
  {"KEY_SYSRQ"                         , 0xFF15},
  {"KEY_SCROLLLOCK"                    , 0xFF14},
  {"KEY_PAUSE"                         , 0xFF13},
  {"KEY_INSERT"                        , 0xFF63},
  {"KEY_HOME"                          , 0xFF50},
  {"KEY_PAGEUP"                        , 0xFF55},
  {"KEY_DELETE"                        , 0xFFFF},
  {"KEY_END"                           , 0xFF57},
  {"KEY_PAGEDOWN"                      , 0xFF56},
  {"KEY_RIGHT"                         , 0xFF53},
  {"KEY_LEFT"                          , 0xFF51},
  {"KEY_DOWN"                          , 0xFF54},
  {"KEY_UP"                            , 0xFF52},
  {"KEY_NUMLOCK"                       , 0xFF7F},


  {"KEY_KPMINUS"                       , 0xFFAD},
  {"KEY_KPPLUS"                        , 0xFFAB},
  {"KEY_KPENTER"                       , 0xFF8D},
  {"KEY_KP1"                           , 0xFFB1},
  {"KEY_KP2"                           , 0xFFB2},
  {"KEY_KP3"                           , 0xFFB3},
  {"KEY_KP4"                           , 0xFFB4},
  {"KEY_KP5"                           , 0xFFB5},
  {"KEY_KP6"                           , 0xFFB6},
  {"KEY_KP7"                           , 0xFFB7},
  {"KEY_KP8"                           , 0xFFB8},
  {"KEY_KP9"                           , 0xFFB9},
  {"KEY_KP0"                           , 0xFFB0},




  {"KEY_KPEqual"                       , 0xFFBD},
  {"KEY_F13"                           , 0xFFCA},
  {"KEY_F14"                           , 0xFFCB},
  {"KEY_F15"                           , 0xFFCC},
  {"KEY_F16"                           , 0xFFCD},
  {"KEY_F17"                           , 0xFFCE},
  {"KEY_F18"                           , 0xFFCF},
  {"KEY_F19"                           , 0xFFD0},
  {"KEY_F20"                           , 0xFFD1},
  {"KEY_F21"                           , 0xFFD2},
  {"KEY_F22"                           , 0xFFD3},
  {"KEY_F23"                           , 0xFFD4},
  {"KEY_F24"                           , 0xFFD5},
  //{"KEY_less"                          , 0x003C},
  //{"KEY_numbersign"                    , 0x0023},






























































































  {"KEY_LEFTCTRL"                      , 0xFFE3},
  {"KEY_LEFTSHIFT"                     , 0xFFE1},
  {"KEY_LEFTALT"                       , 0xFFE9},
  {"KEY_LEFTMETA"                      , 0xFFE7},
  {"KEY_RIGHTCTRL"                     , 0xFFE4},
  {"KEY_RIGHTSHIFT"                    , 0xFFE2},
  {"KEY_RIGHTALT"                      , 0xFFEA},
  {"KEY_RIGHTMETA"                     , 0xFFE8},

// workaround for linux
  {"KEY_F13"                           , 0x1008FF81},
  {"KEY_F14"                           , 0x1008FF45},
  {"KEY_F15"                           , 0x1008FF46},
  {"KEY_F16"                           , 0x1008FF47},
  {"KEY_F17"                           , 0x1008FF48},
  {"KEY_F18"                           , 0x1008FF49},
  {"KEY_F19"                           , 0x00000000},
  {"KEY_F20"                           , 0x1008FFB2},
  {"KEY_F21"                           , 0x1008FFA9},
  {"KEY_F22"                           , 0x1008FFB0},
  {"KEY_F23"                           , 0x1008FFB1},
  {"KEY_F24"                           , 0x00000000},
  };

}


#ifndef FX_NO_GLOBAL_NAMESPACE
using namespace FX;
#endif


#endif
