/*
Copyright (c) 2015 Sean Bohan

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

#pragma once

#include "MuGlobals.h"
#include "MuEvent.h"

namespace mural {

    class MouseEvent : public Event {
        public:
            // "type"
            static const int MOUSE_DOWN = 1;
            static const int MOUSE_MOVE = 2;
            static const int MOUSE_UP = 3;
            // "which"
            static const int WHICH_NO_BUTTON = 0;
            static const int WHICH_LEFT = 1;
            static const int WHICH_MIDDLE = 2;
            static const int WHICH_RIGHT = 3;
            // "buttons"
            static const int BUTTONS_LEFT = 1;
            static const int BUTTONS_RIGHT = 2;
            static const int BUTTONS_MIDDLE = 4;

            // Properties
            int type;
            int which;
            int buttons;
            bool altDown;
            bool ctrlDown;
            bool metaDown;
            bool shiftDown;
            int x, y;
            int movementX, movementY;

            MouseEvent();
    };

    class KeyboardEvent : public Event {
        public:
            // "type"
            static const int KEY_DOWN = 1;
            static const int KEY_PRESS = 2;
            static const int KEY_UP = 3;

            // Properties
            int type;
            int keyCode;
            const String code;

            KeyboardEvent(int type, int key);
    };

}
