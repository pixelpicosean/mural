/*
 Copyright (C) 2011 by Ivan Safrin

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "MuInput.h"
#include "MuInputEvent.h"

namespace mural {

    JoystickInfo::JoystickInfo() {
        for (int i = 0; i < 32; i++) {
            joystickAxisState[i] = 0.0;
        }

        for (int i = 0; i < 64; i++) {
            joystickButtonState[i] = false;
        }
    }

    MuInput::MuInput() : EventDispatcher() {
        clearInput();
        simulateTouchWithMouse = false;
        simulateMouseWithTouch = false;
        ignoreOffScreenTouch = false;
        keyRepeat = true;
    }

    void MuInput::clearInput() {
        mouseButtons[0] = false;
        mouseButtons[1] = false;
        mouseButtons[2] = false;

        for (int i = 0; i < 512; i++) {
            keyboardState[i] = 0;
        }
    }

    MuInput::~MuInput() {}

    unsigned int MuInput::getNumJoysticks() {
        return joysticks.size();
    }

    JoystickInfo *MuInput::getJoystickInfoByIndex(unsigned int index) {
        if (index > joysticks.size() - 1 || joysticks.size() == 0) {
            return NULL;
        }
        return &joysticks[index];
    }

    bool MuInput::getJoystickButtonState(int joystickIndex, int button) {
        JoystickInfo *info = getJoystickInfoByIndex(joystickIndex);
        if (info) {
            return info->joystickButtonState[button];
        }
        else {
            return false;
        }
    }

    Number MuInput::getJoystickAxisValue(int joystickIndex, int axis) {
        JoystickInfo *info = getJoystickInfoByIndex(joystickIndex);
        if (info) {
            return info->joystickAxisState[axis];
        }
        else {
            return 0.0;
        }
    }

    JoystickInfo *MuInput::getJoystickInfoByID(unsigned int deviceID) {
        for (unsigned int i = 0; i<joysticks.size(); i++) {
            if (joysticks[i].deviceID == deviceID) {
                joysticks[i].deviceIndex = i;
                return &joysticks[i];
            }
        }
        return NULL;
    }

    void MuInput::addJoystick(unsigned int deviceID) {
        JoystickInfo joystick;
        joystick.deviceID = deviceID;
        joysticks.push_back(joystick);
        InputEvent *evt = new InputEvent();
        evt->joystickDeviceID = deviceID;
        evt->joystickIndex = joysticks.size()-1;
        dispatchEvent(evt, InputEvent::EVENT_JOYDEVICE_ATTACHED);
    }

    void MuInput::removeJoystick(unsigned int deviceID) {
        for(int i=0;i<joysticks.size();i++) {
            if(joysticks[i].deviceID == deviceID) {
                joysticks.erase(joysticks.begin()+i);
                InputEvent *evt = new InputEvent();
                evt->joystickDeviceID = deviceID;
                evt->joystickIndex = i;
                dispatchEvent(evt, InputEvent::EVENT_JOYDEVICE_DETACHED);
                return;
            }
        }
    }

    void MuInput::joystickAxisMoved(unsigned int axisID, float value, unsigned int deviceID) {
        JoystickInfo *info = getJoystickInfoByID(deviceID);
        if(info) {
            info->joystickAxisState[axisID] = value;
            InputEvent *evt = new InputEvent();
            evt->joystickDeviceID = deviceID;
            evt->joystickAxis = axisID;
            evt->joystickAxisValue = value;
            evt->joystickIndex = info->deviceIndex;
            dispatchEvent(evt, InputEvent::EVENT_JOYAXIS_MOVED);
        }
    }

    void MuInput::joystickButtonDown(unsigned int buttonID, unsigned int deviceID) {
        JoystickInfo *info = getJoystickInfoByID(deviceID);
        if(info) {
            info->joystickButtonState[buttonID] = true;
            InputEvent *evt = new InputEvent();
            evt->joystickDeviceID = deviceID;
            evt->joystickButton = buttonID;
            evt->joystickIndex = info->deviceIndex;
            dispatchEvent(evt, InputEvent::EVENT_JOYBUTTON_DOWN);
        }
    }

    void MuInput::joystickButtonUp(unsigned int buttonID, unsigned int deviceID) {
        JoystickInfo *info = getJoystickInfoByID(deviceID);
        if(info) {
            info->joystickButtonState[buttonID] = false;
            InputEvent *evt = new InputEvent();
            evt->joystickDeviceID = deviceID;
            evt->joystickButton = buttonID;
            evt->joystickIndex = info->deviceIndex;
            dispatchEvent(evt, InputEvent::EVENT_JOYBUTTON_UP);
        }
    }


    bool MuInput::getMouseButtonState(int mouseButton) {
        return mouseButtons[mouseButton];
    }

    void MuInput::setMouseButtonState(int mouseButton, bool state, int ticks) {
        InputEvent *evt = new InputEvent(mousePosition, ticks);
        evt->mouseButton = mouseButton;
        if(state)
            dispatchEvent(evt, InputEvent::EVENT_MOUSEDOWN);
        else
            dispatchEvent(evt, InputEvent::EVENT_MOUSEUP);
        mouseButtons[mouseButton] = state;

        if(simulateTouchWithMouse) {
            TouchInfo touch;
            touch.position = mousePosition;
            touch.id = 0;
            std::vector<TouchInfo> touches;
            touches.push_back(touch);

            if(state) {
                touchesBegan(touch, touches, ticks);
            } else {
                touchesEnded(touch, touches, ticks);
            }
        }
    }

    void MuInput::mouseWheelDown(int ticks) {
        InputEvent *evt = new InputEvent(mousePosition, ticks);
        dispatchEvent(evt, InputEvent::EVENT_MOUSEWHEEL_DOWN);
    }

    void MuInput::mouseWheelUp(int ticks) {
        InputEvent *evt = new InputEvent(mousePosition, ticks);
        dispatchEvent(evt, InputEvent::EVENT_MOUSEWHEEL_UP);
    }

    void MuInput::setMousePosition(int x, int y, int ticks) {
        mousePosition.x = x;
        mousePosition.y = y;
        InputEvent *evt = new InputEvent(mousePosition, ticks);
        dispatchEvent(evt, InputEvent::EVENT_MOUSEMOVE);

        if(simulateTouchWithMouse) {



            TouchInfo touch;
            touch.position = mousePosition;
            touch.id = 0;
            std::vector<TouchInfo> touches;
            touches.push_back(touch);

            if(!mouseButtons[MOUSE_BUTTON1]) {
                mouseButtons[MOUSE_BUTTON1] = true;
                touchesBegan(touch, touches, ticks);
            }
            touchesMoved(touch, touches, ticks);
        }
    }

    vec2 MuInput::getMouseDelta() {
        return deltaMousePosition;
    }

    void MuInput::setDeltaPosition(int x, int y) {
        deltaMousePosition.x = (Number)x;
        deltaMousePosition.y = (Number)y;
    }

    vec2 MuInput::getMousePosition() {
        return mousePosition;
    }

    bool MuInput::getKeyState(MuKEY keyCode) {
        if (keyCode < 512)
            return keyboardState[keyCode];
        else
            return false;
    }

    void MuInput::setKeyState(MuKEY keyCode, wchar_t code, bool newState, int ticks) {

        if(newState && !keyRepeat) {
            if(keyboardState[keyCode]) {
                return;
            }
        }

        InputEvent *evt = new InputEvent(keyCode, code, ticks);
        if(keyCode < 512)
            keyboardState[keyCode] = newState;
        if(newState) {
            dispatchEvent(evt, InputEvent::EVENT_KEYDOWN);
        } else {
            dispatchEvent(evt, InputEvent::EVENT_KEYUP);
        }
    }

    void MuInput::touchesBegan(TouchInfo touch, std::vector<TouchInfo> touches, int ticks) {
        /*if(ignoreOffScreenTouch) {
            Core *core = CoreServices::getInstance()->getCore();
            if(touch.position.x < 0 || touch.position.x > core->getXRes() || touch.position.y < 0 || touch.position.y > core->getYRes()) {
                return;
            }
        }
        InputEvent *evt = new InputEvent();
        evt->touch = touch;
        evt->touches = touches;
        evt->timestamp = ticks;
        dispatchEvent(evt, InputEvent::EVENT_TOUCHES_BEGAN);
        if(simulateMouseWithTouch) {
            mousePosition = touch.position;
            setMouseButtonState(MOUSE_BUTTON1, true, ticks);
        }*/
    }

    void MuInput::touchesMoved(TouchInfo touch, std::vector<TouchInfo> touches, int ticks) {
        /*if(ignoreOffScreenTouch) {
            Core *core = CoreServices::getInstance()->getCore();
            if(touch.position.x < 0 || touch.position.x > core->getXRes() || touch.position.y < 0 || touch.position.y > core->getYRes()) {
                return;
            }
        }
        InputEvent *evt = new InputEvent();
        evt->touch = touch;
        evt->touches = touches;
        evt->timestamp = ticks;
        dispatchEvent(evt, InputEvent::EVENT_TOUCHES_MOVED);
        if(simulateMouseWithTouch) {
            setMousePosition(touch.position.x, touch.position.y, ticks);
        }*/
    }

    void MuInput::touchesEnded(TouchInfo touch, std::vector<TouchInfo> touches, int ticks) {
        /*if(ignoreOffScreenTouch) {
            Core *core = CoreServices::getInstance()->getCore();
            if(touch.position.x < 0 || touch.position.x > core->getXRes() || touch.position.y < 0 || touch.position.y > core->getYRes()) {
                return;
            }
        }
        InputEvent *evt = new InputEvent();
        evt->touch = touch;
        evt->touches = touches;
        evt->timestamp = ticks;
        dispatchEvent(evt, InputEvent::EVENT_TOUCHES_ENDED);
        if(simulateMouseWithTouch) {
            mousePosition = touch.position;
            setMouseButtonState(MOUSE_BUTTON1, false, ticks);
        }*/
    }

}
