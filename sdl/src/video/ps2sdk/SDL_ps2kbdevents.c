/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

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
    PS2 keyboard events manager
 	Shazz / MJJ prod 	
*/

#include <string.h>

#include "SDL.h"
#include "SDL_sysevents.h"
#include "SDL_events_c.h"
#include "SDL_ps2kbdevents.h"

//public Interface
#include "SDL_ps2keys.h"

//Common PS2 types
#include <tamtypes.h>

//IRX loader
#include <loadfile.h>

//Keyboard driver lib
#include <libkbd.h>

extern u8 ps2kbd_irx[];
extern int size_ps2kbd_irx;

//Keyboard state
#define KBD_AVAILABLE 1
#define KBD_NOT_AVAILABLE 0
static int kbdState = KBD_NOT_AVAILABLE;

/* The translation tables from a console scancode to a SDL keysym */
static SDLKey keymap[240];
static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym);


/*
 Callback, triggered at init to define specific OS key mapping
 */
void PS2_InitOSKeymap(_THIS)
{
	int i;

	/* Initialize the PS2 key translation table */

	/* First get the ascii keys and others not well handled */
	for (i=0; i<SDL_TABLESIZE(keymap); i++) 
	{
		switch(i) 
		{
		/* These aren't handled by the x86 kernel keymapping (?) */
		case SCANCODE_PRINTSCREEN:
		keymap[i] = SDLK_PRINT;
		break;
		
		case SCANCODE_BREAK:
		keymap[i] = SDLK_BREAK;
		break;
		
		case SCANCODE_BREAK_ALTERNATIVE:
		keymap[i] = SDLK_PAUSE;
		break;
		
		case SCANCODE_LEFTSHIFT:
		keymap[i] = SDLK_LSHIFT;
		break;
		
		case SCANCODE_RIGHTSHIFT:
		keymap[i] = SDLK_RSHIFT;
		break;
		
		case SCANCODE_LEFTCONTROL:
		keymap[i] = SDLK_LCTRL;
		break;
		
		case SCANCODE_RIGHTCONTROL:
		keymap[i] = SDLK_RCTRL;
		break;
		
		case SCANCODE_RIGHTWIN:
		keymap[i] = SDLK_RSUPER;    
		break;
		
		case SCANCODE_LEFTWIN:	        
		keymap[i] = SDLK_LSUPER;    
		break;
		
		case SCANCODE_LEFTALT:            
		keymap[i] = SDLK_LALT;      
		break;
		
		case SCANCODE_RIGHTALT:           
		keymap[i] = SDLK_RALT;      
		break;	  
		
		case SCANCODE_TAB:                
		keymap[i] = SDLK_TAB;       
		break;
		
		case SCANCODE_SPACE:              
		keymap[i] = SDLK_SPACE;     
		break;
		
		case SCANCODE_INSERT:             
		keymap[i] = SDLK_INSERT;    
		break;
		
		case SCANCODE_REMOVE:             
		keymap[i] = SDLK_DELETE;    
		break;
		
		case SCANCODE_PAGEUP:             
		keymap[i] = SDLK_PAGEUP;    
		break;
		
		case SCANCODE_PAGEDOWN:           
		keymap[i] = SDLK_PAGEDOWN;  
		break;
		
		case SCANCODE_HOME:               
		keymap[i] = SDLK_HOME;      
		break;
		
		case SCANCODE_END:                
		keymap[i] = SDLK_END;       
		break;
		
		case SCANCODE_NUMLOCK:            
		keymap[i] = SDLK_NUMLOCK;   
		break;
		
		case SCANCODE_CAPSLOCK:           
		keymap[i] = SDLK_CAPSLOCK;  
		break;
		
		case SCANCODE_SCROLLLOCK:         
		keymap[i] = SDLK_SCROLLOCK; 
		break;
		
		case SCANCODE_BACKSPACE:          
		keymap[i] = SDLK_BACKSPACE; 
		break;
		
		case SCANCODE_F1:                 
		keymap[i] = SDLK_F1;        
		break;
		
		case SCANCODE_F2:                 
		keymap[i] = SDLK_F2;        
		break;
		
		case SCANCODE_F3:                 
		keymap[i] = SDLK_F3;        
		break;
		
		case SCANCODE_F4:                 
		keymap[i] = SDLK_F4;        
		break;
		
		case SCANCODE_F5:                 
		keymap[i] = SDLK_F5;        
		break;
		
		case SCANCODE_F6:                 
		keymap[i] = SDLK_F6;        
		break;
		
		case SCANCODE_F7:                 
		keymap[i] = SDLK_F7;        
		break;
		
		case SCANCODE_F8:                 
		keymap[i] = SDLK_F8;        
		break;
		
		case SCANCODE_F9:                 
		keymap[i] = SDLK_F9;        
		break;
		
		case SCANCODE_F10:                
		keymap[i] = SDLK_F10;       
		break;
		
		case SCANCODE_F11:                
		keymap[i] = SDLK_F11;       
		break;
		
		case SCANCODE_F12:                
		keymap[i] = SDLK_F12;       
		break;
		
		case SCANCODE_CURSORDOWN:         
		keymap[i] = SDLK_DOWN;      
		break;
		
		case SCANCODE_CURSORLEFT:         
		keymap[i] = SDLK_LEFT;      
		break;
		
		case SCANCODE_CURSORRIGHT:        
		keymap[i] = SDLK_RIGHT;     
		break;
		
		case SCANCODE_CURSORUP:           
		keymap[i] = SDLK_UP;        
		break;
		
		case SCANCODE_KEYPAD0:            
		keymap[i] = SDLK_KP0;       
		break;
		
		case SCANCODE_KEYPAD1:            
		keymap[i] = SDLK_KP1;       
		break;
		
		case SCANCODE_KEYPAD2:            
		keymap[i] = SDLK_KP2;       
		break;
		
		case SCANCODE_KEYPAD3:            
		keymap[i] = SDLK_KP3;       
		break;
		
		case SCANCODE_KEYPAD4:            
		keymap[i] = SDLK_KP4;       
		break;
		
		case SCANCODE_KEYPAD5:            
		keymap[i] = SDLK_KP5;       
		break;
		
		case SCANCODE_KEYPAD6:            
		keymap[i] = SDLK_KP6;       
		break;
		
		case SCANCODE_KEYPAD7:            
		keymap[i] = SDLK_KP7;       
		break;
		
		case SCANCODE_KEYPAD8:            
		keymap[i] = SDLK_KP8;       
		break;
		
		case SCANCODE_KEYPAD9:            
		keymap[i] = SDLK_KP9;       
		break;
		
		case SCANCODE_KEYPADPLUS:         
		keymap[i] = SDLK_KP_PLUS;   
		break;
		case SCANCODE_KEYPADMINUS:        
		keymap[i] = SDLK_KP_MINUS;  
		break;
		
		case SCANCODE_KEYPADMULTIPLY:     
		keymap[i] = SDLK_KP_MULTIPLY; 
		break;
		
		case SCANCODE_KEYPADDIVIDE:       
		keymap[i] = SDLK_KP_DIVIDE; 
		break;
		
		case SCANCODE_KEYPADENTER:        
		keymap[i] = SDLK_KP_ENTER;  
		break;
		
		case SCANCODE_KEYPADPERIOD:       
		keymap[i] = SDLK_KP_PERIOD; 
		break;
		
		case SCANCODE_ESCAPE:             
		keymap[i] = SDLK_ESCAPE;    
		break;
		
		case SCANCODE_MINUS:              
		keymap[i] = SDLK_MINUS;     
		break;
		
		case SCANCODE_EQUAL:              
		keymap[i] = SDLK_EQUALS;    
		break;
		
		case SCANCODE_BRACKET_LEFT:       
		keymap[i] = SDLK_LEFTBRACKET;    
		break;
		
		case SCANCODE_BRACKET_RIGHT:      
		keymap[i] = SDLK_RIGHTBRACKET;   
		break;
		
		case SCANCODE_ENTER:              
		keymap[i] = SDLK_RETURN;    
		break;
		
		case SCANCODE_SEMICOLON:          
		keymap[i] = SDLK_SEMICOLON; 
		break;
		
		case SCANCODE_APOSTROPHE:         
		keymap[i] = SDLK_QUOTE;     
		break;
		
		case SCANCODE_GRAVE:              
		keymap[i] = SDLK_BACKQUOTE; 
		break;
		
		case SCANCODE_COMMA:              
		keymap[i] = SDLK_COMMA;     
		break;
		
		case SCANCODE_PERIOD:             
		keymap[i] = SDLK_PERIOD;    
		break;
      
      //shifted chars to do...
      	     
	  /* this should take care of standard ascii keys */
	  default:
        if(i>= 4  && i<=29 )     keymap[i] = (i+93);  // a-z
        if(i>= 30 && i<= 38)     keymap[i] = (i+19);  // 1-9
        if(i == 39)              keymap[i] = 48;      //0
	    break;
	  }
	}    
}

/*
 Load the embedded USB keyboard driver and then initialize the keyboard
 */
int PS2_InitKeyboard(_THIS){
    
    int ret;
    
    printf("[PS2] Init USB Keyboard\n");
    
    // the keyboard driver is embedded in the library....
    SifExecModuleBuffer(ps2kbd_irx, size_ps2kbd_irx, 0, NULL, &ret);
    if (ret < 0) {
           SDL_SetError("[PS2] Failed to load module: ps2kbd_irx\n");
           return -1;
    } 
    else {
    
        if((PS2KbdInit()) == 0) {
            SDL_SetError("[PS2] PS2KbdInit failed\n");
            kbdState = KBD_NOT_AVAILABLE;
            
            return -1;
        } else {
            
            PS2KbdSetReadmode(PS2KBD_READMODE_RAW);
            PS2KbdSetBlockingMode(PS2KBD_NONBLOCKING);
            kbdState = KBD_AVAILABLE;
        }
    }     
    
    printf("[PS2] Init USB Keyboard done\n");
    return 0;  
       
}

/*
 Callback function used in video.c to delegate keyboard events
*/
void PS2_PumpKeyboardEvents(_THIS)
{
	if (kbdState == KBD_AVAILABLE)
	{
		SDL_keysym keysym;
		
		PS2KbdRawKey key;
		int keyState;
		int scanCode;
		
		// Read USB keyboard
		PS2KbdReadRaw(&key);
		
		if (key.key != 0) 
		{
			// Check key state      
			if (key.state == PS2KBD_RAWKEY_DOWN) 
			{
				keyState = SDL_PRESSED;
			} 
			else 
			{
				keyState = SDL_RELEASED;
			}
		
			// Get key scancode
			scanCode = key.key;
		
			// Translate this scancode to SDL keymap
			TranslateKey(scanCode, &keysym);
		
			// Send SDL key event
			SDL_PrivateKeyboard(keyState, &keysym);         
		}  
	}      
}

/*
 Private translation [PS2->SDL] fonction using the PS2 keymap 
*/
static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym)
{
	/* Set the keysym information using keymap[scancode->SDL] */
	keysym->scancode = scancode;
	keysym->sym = keymap[scancode];
	keysym->mod = KMOD_NONE;
	keysym->unicode = 0;

	return(keysym);
}
