/*
	SDL - Simple DirectMedia Layer
	Copyright (C) 1997, 1998, 1999, 2000  Sam Lantinga

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Sam Lantinga
	slouken@libsdl.org
*/

/*
 Scancodes for the PS2 USB Datel Keyboard, US only
 Shazz / MJJ prod
 
 Could be nice to have diffrent keymap for french, english... keymaps
*/

/*
This is my keyboard... in line.... it helps :D
static const unsigned char keymap_US[] = {
            41, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
            53, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 49, 42, 73, 74, 75, 83, 84, 85, 86,
            43, 20, 26, 8, 21, 23, 28, 24, 12, 18, 19, 47, 48, 76, 77, 78, 95, 96, 97,
            57, 4, 22, 7, 9, 10, 11, 13, 14, 15, 51, 52, 40, 92, 93, 94, 87,
            225, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 229, 82, 89, 90, 91,
            224, 227, 226, 44, 230, 231, 101, 228, 80, 81, 79, 98, 99, 88
};*/

#define SCANCODE_ESCAPE		     41

#define SCANCODE_F1			     58
#define SCANCODE_F2			     59
#define SCANCODE_F3			     60
#define SCANCODE_F4			     61
#define SCANCODE_F5			     62
#define SCANCODE_F6			     63
#define SCANCODE_F7			     64
#define SCANCODE_F8			     65
#define SCANCODE_F9			     66
#define SCANCODE_F10			 67
#define SCANCODE_F11			 68
#define SCANCODE_F12			 69
        
#define SCANCODE_1			     30
#define SCANCODE_2			     31
#define SCANCODE_3			     32
#define SCANCODE_4		      	 33
#define SCANCODE_5		       	 34
#define SCANCODE_6			     35
#define SCANCODE_7			     36
#define SCANCODE_8			     37
#define SCANCODE_9			     38
#define SCANCODE_0			     39

#define SCANCODE_MINUS		     45
#define SCANCODE_EQUAL		     46

#define SCANCODE_BACKSPACE	     42
#define SCANCODE_TAB		     43

#define SCANCODE_Q			     20
#define SCANCODE_W			     26
#define SCANCODE_E			     8
#define SCANCODE_R			     21
#define SCANCODE_T			     23
#define SCANCODE_Y			     28
#define SCANCODE_U			     24
#define SCANCODE_I			     12
#define SCANCODE_O			     18
#define SCANCODE_P			     19
#define SCANCODE_BRACKET_LEFT	 47
#define SCANCODE_BRACKET_RIGHT	 48

#define SCANCODE_ENTER		     40

#define SCANCODE_A			     4
#define SCANCODE_S			     22
#define SCANCODE_D			     7
#define SCANCODE_F			     9
#define SCANCODE_G			     10
#define SCANCODE_H			     11
#define SCANCODE_J			     13
#define SCANCODE_K			     14
#define SCANCODE_L			     15
#define SCANCODE_SEMICOLON		 51
#define SCANCODE_APOSTROPHE		 52
#define SCANCODE_GRAVE			 53

#define SCANCODE_Z			     29
#define SCANCODE_X			     27
#define SCANCODE_C			     6
#define SCANCODE_V			     25
#define SCANCODE_B			     5
#define SCANCODE_N			     17
#define SCANCODE_M			     16
#define SCANCODE_COMMA			 54
#define SCANCODE_PERIOD			 55
#define SCANCODE_SLASH			 56

#define SCANCODE_LEFTCONTROL	 224
#define SCANCODE_LEFTSHIFT		 225
#define SCANCODE_BACKSLASH		 43
#define SCANCODE_RIGHTSHIFT		 229
#define SCANCODE_LEFTALT		 226
#define SCANCODE_SPACE			 44
#define SCANCODE_CAPSLOCK		 57

#define SCANCODE_RIGHTCONTROL	 228
#define SCANCODE_CONTROL		 224
#define SCANCODE_PRINTSCREEN	 70
#define SCANCODE_RIGHTALT		 230
#define SCANCODE_BREAK			 72	
#define SCANCODE_BREAK_ALTERNATIVE	0	

#define SCANCODE_NUMLOCK		 83
#define SCANCODE_SCROLLLOCK		 71

#define SCANCODE_KEYPADMULTIPLY	 85
#define SCANCODE_KEYPADMINUS	 86
#define SCANCODE_KEYPADPLUS		 87
#define SCANCODE_KEYPADPERIOD	 99
#define SCANCODE_KEYPADENTER	 88
#define SCANCODE_KEYPADDIVIDE	 84

#define SCANCODE_KEYPAD7		 95
#define SCANCODE_KEYPAD8		 96
#define SCANCODE_KEYPAD9		 97
#define SCANCODE_KEYPAD4		 92
#define SCANCODE_KEYPAD5		 93
#define SCANCODE_KEYPAD6		 94
#define SCANCODE_KEYPAD1		 89
#define SCANCODE_KEYPAD2		 90
#define SCANCODE_KEYPAD3		 91
#define SCANCODE_KEYPAD0		 98

#define SCANCODE_CURSORLEFT		 80
#define SCANCODE_CURSORRIGHT	 79
#define SCANCODE_CURSORUP		 82
#define SCANCODE_CURSORDOWN		 81

#define SCANCODE_CURSORUPLEFT	 0
#define SCANCODE_CURSORUPRIGHT	 0
#define SCANCODE_CURSORDOWNLEFT	 0
#define SCANCODE_CURSORDOWNRIGHT 0

#define SCANCODE_LESS			 0

#define SCANCODE_HOME			  74
#define SCANCODE_PAGEUP			  75
#define SCANCODE_END			  77
#define SCANCODE_PAGEDOWN		  78
#define SCANCODE_INSERT			  73
#define SCANCODE_REMOVE			  76

#define SCANCODE_CURSORBLOCKUP	  0
#define SCANCODE_CURSORBLOCKLEFT  0	
#define SCANCODE_CURSORBLOCKRIGHT 0	
#define SCANCODE_CURSORBLOCKDOW	  0

#define SCANCODE_RIGHTWIN		  231
#define SCANCODE_LEFTWIN		  227

