#include "KeyEventHandler.hpp"

#include "KeyEvent.hpp"

#include <hardware/Hardware.hpp>

#include <sys/OsxKeyCodes.hpp>
#include <Logger.hpp>

#include <sstream>
#include <iomanip>

using namespace mpc::controls;
using namespace mpc::hardware;
using namespace std;

KeyEventHandler::KeyEventHandler(weak_ptr<Hardware> hardware)
    : hardware (hardware)
{
}

void KeyEventHandler::handle(const KeyEvent& keyEvent)
{

    auto it = find(begin(pressed), end(pressed), keyEvent.rawKeyCode);

    bool isCapsLock = false;
#ifdef __APPLE__
    isCapsLock = moduru::sys::OsxKeyCodes::keyCodeNames[keyEvent.rawKeyCode].compare("caps lock") == 0;
#endif

    // Special case as caps lock only sends key releases on OSX
    if (isCapsLock) {
        if (it == end(pressed))
        {
            // We must fire a "push" to whatever control caps lock is bound to.
            pressed.push_back(keyEvent.rawKeyCode);
        } else {
            // We must fire a "release" to whatever control caps lock is bound to.
            pressed.erase(it);
        }
    }
    else {
        if (keyEvent.keyDown) {
            
            if (it != end(pressed))
            {
                // For now we don't accept any kind of auto-repeat.
                // Maybe in the future when we allow users to type things like sound
                // and sequence names with the keyboard we will.
                return;
            }
            
            pressed.push_back(keyEvent.rawKeyCode);
            
        } else {
            pressed.erase(it);
        }
    }
    
    stringstream rawKeyCodeHex;
    rawKeyCodeHex << std::hex << keyEvent.rawKeyCode;
    auto rawKeyCodeHexString = string(rawKeyCodeHex.str());
    MLOG("KeyEventHandler::handle key " + string(keyEvent.keyDown ? "press" : "release") + " event with rawKeyCode " + to_string(keyEvent.rawKeyCode) + ", " + rawKeyCodeHexString);

}
