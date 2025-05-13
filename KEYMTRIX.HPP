#ifndef __KEYMTRIX_HPP__
#define __KEYMTRIX_HPP__

// this is not working in dos4gw, in !!!!!pmode/w!!!!! it's working

extern bool keyPressed[0x80];
bool isKeyPressed(int scanCode);

void installKeyboardHandler();
void uninstallKeyboardHandler();

#define SCANCODE_ESCAPE 1
#define SCANCODE_1 2
#define SCANCODE_2 3
#define SCANCODE_3 4
#define SCANCODE_4 5
#define SCANCODE_5 6
#define SCANCODE_6 7
#define SCANCODE_7 8
#define SCANCODE_8 9
#define SCANCODE_9 10
#define SCANCODE_0 11
#define SCANCODE_QUESTION 12
#define SCANCODE_UPPERDOT 13
#define SCANCODE_BACKSPACE 14
#define SCANCODE_TAB 15
#define SCANCODE_Q 16
#define SCANCODE_W 17
#define SCANCODE_E 18
#define SCANCODE_R 19
#define SCANCODE_T 20
#define SCANCODE_Z 21
#define SCANCODE_U 22
#define SCANCODE_I 23
#define SCANCODE_O 24
#define SCANCODE_P 25
#define SCANCODE_UE 26
#define SCANCODE_PLUS 27
#define SCANCODE_ENTER 28
#define SCANCODE_NUMENTER 28
#define SCANCODE_PAUSE 29
#define SCANCODE_CTRL 29
#define SCANCODE_A 30
#define SCANCODE_S 31
#define SCANCODE_D 32
#define SCANCODE_F 33
#define SCANCODE_G 34
#define SCANCODE_H 35
#define SCANCODE_J 36
#define SCANCODE_K 37
#define SCANCODE_L 38
#define SCANCODE_OE 39
#define SCANCODE_AE 40
#define SCANCODE_DACH 41
#define SCANCODE_LSHIFT 42
#define SCANCODE_HASHTAG 43
#define SCANCODE_Y 44
#define SCANCODE_X 45
#define SCANCODE_C 46
#define SCANCODE_V 47
#define SCANCODE_B 48
#define SCANCODE_N 49
#define SCANCODE_M 50
#define SCANCODE_COMMA 51
#define SCANCODE_DOT 52
#define SCANCODE_NUMDIVIDE 53
#define SCANCODE_MINUS 53
#define SCANCODE_RSHIFT 54
#define SCANCODE_NUMMUL 55
#define SCANCODE_PRINT 55
#define SCANCODE_ALT 56
#define SCANCODE_SPACE 57
#define SCANCODE_SHIFTLOCK 58
#define SCANCODE_F1 59
#define SCANCODE_F2 60
#define SCANCODE_F3 61
#define SCANCODE_F4 62
#define SCANCODE_F5 63
#define SCANCODE_F6 64
#define SCANCODE_F7 65
#define SCANCODE_F8 66
#define SCANCODE_F9 67
#define SCANCODE_F10 68
#define SCANCODE_NUMNUM 69
#define SCANCODE_ROLL 70
#define SCANCODE_START 71
#define SCANCODE_NUM8 72
#define SCANCODE_UP 72
#define SCANCODE_NUM7 71
#define SCANCODE_NUM9 73
#define SCANCODE_PAGEUP 73
#define SCANCODE_NUMMINUS 74
#define SCANCODE_NUM4 75
#define SCANCODE_LEFT 75
#define SCANCODE_NUM5 76
#define SCANCODE_NUM6 77
#define SCANCODE_RIGHT 77
#define SCANCODE_NUMPLUS 78
#define SCANCODE_NUM1 79
#define SCANCODE_END 79
#define SCANCODE_NUM2 80
#define SCANCODE_DOWN 80
#define SCANCODE_NUM3 81
#define SCANCODE_PAGEDOWN 81
#define SCANCODE_INSERT 82
#define SCANCODE_NUM0 82
#define SCANCODE_NUMCOMMA 83
#define SCANCODE_DELETE 83
#define SCANCODE_SMALLER 86
#define SCANCODE_F11 87
#define SCANCODE_F12 88
#define SCANCODE_WINDOWS 91

#endif __KEYMTRIX_HPP__
