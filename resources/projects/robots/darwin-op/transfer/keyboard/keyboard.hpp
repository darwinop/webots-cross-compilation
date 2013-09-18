/*
 * File:         keyboard.hpp
 * Date:         January 2013
 * Description:  Class used to read keyboard input with X11
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#define NKEYS 200

#define PRESSED                 1
#define PRESSED_AND_RELEASE     2
#define VALIDATED               3
#define VALIDATED_STILL_PRESSED 4

#define WB_ROBOT_KEYBOARD_KEY     0x000ffff
#define WB_ROBOT_KEYBOARD_SHIFT   0x0010000
#define WB_ROBOT_KEYBOARD_CONTROL 0x0020000
#define WB_ROBOT_KEYBOARD_ALT     0x0040000

#define WB_ROBOT_KEYBOARD_LEFT          314
#define WB_ROBOT_KEYBOARD_UP            315
#define WB_ROBOT_KEYBOARD_RIGHT         316
#define WB_ROBOT_KEYBOARD_DOWN          317
#define WB_ROBOT_KEYBOARD_PAGEUP        366
#define WB_ROBOT_KEYBOARD_PAGEDOWN      367
#define WB_ROBOT_KEYBOARD_HOME          313
#define WB_ROBOT_KEYBOARD_END           312

class Keyboard {
  public:
             Keyboard();
    virtual ~Keyboard() {}
    
    void     createWindow();
    void     closeWindow();
    void     startListenKeyboard();
    void     initialiseKeyPressed();
    void     resetKeyPressed();
    
    int      getKeyPressed();

  private:
    void     setKeyPressed(int key);
    void     setKeyReleased(int key);

    int      mKeyPressed[NKEYS];
};

#endif
