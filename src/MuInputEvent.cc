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
