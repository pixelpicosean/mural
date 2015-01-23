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

#include "MuInputEvent.h"
#include "MuInputKeys.h"

namespace mural {

    MouseEvent::MouseEvent():
        type(0),
        which(WHICH_NO_BUTTON),
        buttons(0),
        altDown(false),
        ctrlDown(false),
        metaDown(false),
        shiftDown(false),
        x(0), y(0),
        movementX(0), movementY(0)
    {}

    KeyboardEvent::KeyboardEvent(int type, int key):
        type(type),
        keyCode(KeyboardKeyCodeMap.at(key)),
        code(KeyboardCodeMap.at(key))
    {}

}
