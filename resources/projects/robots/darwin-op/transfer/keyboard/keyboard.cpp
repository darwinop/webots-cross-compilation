/*
 * File:         keyboard.hpp
 * Date:         January 2013
 * Description:  Class used to read keyboard input with X11
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "keyboard.hpp"

Display *display;
Window   window;
XEvent   Report;
int SpecialKey[10] = { 0, WB_ROBOT_KEYBOARD_HOME, WB_ROBOT_KEYBOARD_LEFT, WB_ROBOT_KEYBOARD_UP, WB_ROBOT_KEYBOARD_RIGHT, WB_ROBOT_KEYBOARD_DOWN, WB_ROBOT_KEYBOARD_PAGEUP, WB_ROBOT_KEYBOARD_PAGEDOWN, WB_ROBOT_KEYBOARD_END };

Keyboard::Keyboard() {
  initialiseKeyPressed();
}

void Keyboard::initialiseKeyPressed() {
  for(int c = 0; c < NKEYS; c++)
    mKeyPressed[c] = 0;
}

void Keyboard::resetKeyPressed() {
  for(int c = 0; c < NKEYS; c++) {
    if(mKeyPressed[c] == PRESSED)
      mKeyPressed[c] = 1;
    else if(mKeyPressed[c] == VALIDATED_STILL_PRESSED)
      mKeyPressed[c] = 1;
    else
      mKeyPressed[c] = 0;
  }
}

void Keyboard::createWindow() {
  int width = 500;
  int height = 250;
  
  // Open a new Window
  display = XOpenDisplay(NULL);
  window = XCreateSimpleWindow(display, RootWindow(display, 0), 1, 1, width, height, 0, BlackPixel (display, 0), WhitePixel(display, 0));
  XStoreName(display, window, "Webots Cross-Compilation : Keyboard inputs"); // Set window title
  GC gc = XCreateGC(display, window, 0, NULL); // this variable will contain the handle to the returned graphics context.
  
  // Set this window as not resizable
  XSizeHints * normal_hints = XAllocSizeHints();
  normal_hints->min_width = width;
  normal_hints->max_width = width;
  normal_hints->min_height = height;
  normal_hints->max_height = height;
  normal_hints->flags = PMaxSize | PMinSize;
  XSetWMSizeHints(display, window, normal_hints, XInternAtom(display, "WM_NORMAL_HINTS", 0));
  
  // Set text in the window
  const char text1[256] = "This window is used to catch keyboard inputs by the controller.";
  const char text2[256] = "Please do not close it, it will be closed automatically at the end of controller.";
  XDrawString(display, window, gc, 50, height/2 - 10, text1, 63);
  XDrawString(display, window, gc, 10, height/2 + 10, text2, 81);
  
  // Show window
  XMapWindow(display, window);
  XFlush(display);
  
  // Select Events
  XSelectInput(display, window, KeyPressMask |  KeyRelease);
}

void Keyboard::closeWindow() {
  XDestroyWindow(display, window);
  XCloseDisplay(display);
}


void Keyboard::startListenKeyboard() {
  while (1)  {
    XNextEvent(display, &Report);
    switch(Report.type) {
		
	  case KeyPress : 
	    if(XLookupKeysym(&Report.xkey, 0) < 123)
	      setKeyPressed(XLookupKeysym(&Report.xkey, 0));
	    else {
          switch(XLookupKeysym(&Report.xkey, 0)) {
            case XK_Home : setKeyPressed(1);
              break;
            case XK_Left : setKeyPressed(2);
              break;
            case XK_Up : setKeyPressed(3);
              break;
            case XK_Right : setKeyPressed(4);
             break;
            case XK_Down : setKeyPressed(5);
              break;
            case XK_Page_Up : setKeyPressed(6);
              break;
            case XK_Page_Down : setKeyPressed(7);
              break;
            case XK_End : setKeyPressed(8);
              break;
          }
        }
        break;
        
      case KeyRelease :
        if(XLookupKeysym(&Report.xkey, 0) < 123)
          setKeyReleased(XLookupKeysym(&Report.xkey, 0));
        else {
          switch(XLookupKeysym(&Report.xkey, 0)) {
            case XK_Home : setKeyReleased(1);
              break;
            case XK_Left : setKeyReleased(2);
              break;
            case XK_Up : setKeyReleased(3);
              break;
            case XK_Right : setKeyReleased(4);
              break;
            case XK_Down : setKeyReleased(5);
              break;
            case XK_Page_Up : setKeyReleased(6);
              break;
            case XK_Page_Down : setKeyReleased(7);
              break;
            case XK_End : setKeyReleased(8);
              break;
          }
        }
        break; 
    }
  }
}

void Keyboard::setKeyPressed(int key) {
  if(mKeyPressed[key] == VALIDATED)
    mKeyPressed[key] = VALIDATED_STILL_PRESSED;
  else if(mKeyPressed[key] == VALIDATED_STILL_PRESSED)
    mKeyPressed[key] = VALIDATED_STILL_PRESSED;
  else
    mKeyPressed[key] = PRESSED;
} 

void Keyboard::setKeyReleased(int key) {
  if(mKeyPressed[key] == PRESSED)
    mKeyPressed[key] = PRESSED_AND_RELEASE;
  else if(mKeyPressed[key] == VALIDATED_STILL_PRESSED)
    mKeyPressed[key] = VALIDATED;
}

int Keyboard::getKeyPressed() {
  for(int c = 0; c < NKEYS; c++) {
    if(mKeyPressed[c] == PRESSED) {
	  mKeyPressed[c] = VALIDATED_STILL_PRESSED;
	  if(c > 0 && c < 10) // 1->10 for special caracters
	    return SpecialKey[c];
	  else if(c > 96 && c < 123)  // Always return ascii caracter of UpperCase
	    return (c - 32);
	  else
	    return c;
	}
	else if(mKeyPressed[c] == PRESSED_AND_RELEASE) {
      mKeyPressed[c] = VALIDATED;
	  if(c > 0 && c < 10) // 1->10 for special caracters
	    return SpecialKey[c];
	  else if(c > 96 && c < 123)  // Always return ascii caracter of UpperCase
	    return (c - 32);
	  else
	    return c;
	}
  }
  return 0;
}
