#include "KeyEventHandler.hpp"

#include <Mpc.hpp>

#include "KeyEvent.hpp"

#include <controls/Controls.hpp>
#include <controls/KbMapping.hpp>

#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/screens/VmpcKeyboardScreen.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwComponent.hpp>
#include <hardware/DataWheel.hpp>

#include <sys/KeyCodes.hpp>

#include <Logger.hpp>

#include <sstream>
#include <iomanip>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace mpc::hardware;
using namespace moduru::sys;
using namespace std;

KeyEventHandler::KeyEventHandler(mpc::Mpc& mpc)
: mpc (mpc)
{
}

void KeyEventHandler::handle(const KeyEvent& keyEvent)
{
    //MLOG("KeyEventHandler::handle keyEvent.rawKeyCode " + to_string(keyEvent.rawKeyCode) + string(keyEvent.keyDown ? " down" : " up"));

    auto screen = mpc.screens->get<VmpcKeyboardScreen>("vmpc-keyboard");
    auto kbMapping = mpc.getControls().lock()->getKbMapping().lock();
    // If no action is associated with some keypress,
    // don't do anything at all, so we avoid unexpected and undefined behaviour.
    if (kbMapping->getLabelFromKeyCode(keyEvent.rawKeyCode).compare("") == 0 &&
        !screen->isLearning())
    {
        return;
    }
    
//    MLOG("Handling, and not ignoring, key code " + to_string(keyEvent.rawKeyCode) + (keyEvent.keyDown ? " down" : " up"));
//    MLOG("");
    auto it = find(begin(pressed), end(pressed), keyEvent.rawKeyCode);
    
    bool isCapsLock = false;
    isCapsLock = KeyCodes::keyCodeNames[keyEvent.rawKeyCode].compare("caps lock") == 0;
    
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

//            Uncomment to disable key repeats
//            if (it != end(pressed))
//                return;
            
            pressed.push_back(keyEvent.rawKeyCode);
            
        } else {
            
            // There are some odd cases where a key down event actually comes in as a key up.
            // Concrete example on OSX: [fn + left shift] produces a key up event when pressed,
            // and another key up event when released.
            if (it != end(pressed))
                pressed.erase(it);
        }
    }
    
    if (mpc.getLayeredScreen().lock()->getCurrentScreenName().compare("vmpc-keyboard") == 0)
    {
        if (!keyEvent.keyDown)
            return;
                
        if (screen->isLearning())
        {
            screen->setLearnCandidate(keyEvent.rawKeyCode);
        }
        else
        {
            if (screen->isLearning())
                return;

            auto desc = KeyCodes::keyCodeNames[keyEvent.rawKeyCode];
            
            if (desc.compare(KeyCodes::names[KeyCodes::MVK_UpArrow]) == 0)
            {
                screen->up();
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_DownArrow]) == 0)
            {
                screen->down();
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F1]) == 0)
            {
                screen->function(0);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F2]) == 0)
            {
                screen->function(1);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F3]) == 0)
            {
                screen->function(2);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F4]) == 0)
            {
                screen->function(3);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F5]) == 0)
            {
                screen->function(4);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_F6]) == 0)
            {
                screen->function(5);
            }
            else if (desc.compare(KeyCodes::names[KeyCodes::MVK_Escape]) == 0)
            {
                screen->mainScreen();
            }
        }
        
        return;
    }
    
    auto label = kbMapping->getLabelFromKeyCode(keyEvent.rawKeyCode);
    
    //MLOG("Label associated with key code: " + label);
    
    auto hardwareComponent = mpc.getHardware().lock()->getComponentByLabel(label).lock();
    
    if (hardwareComponent)
    {
        if (keyEvent.keyDown)
        {
            if ((label.length() == 5 || label.length() == 6) && label.find("pad-") != string::npos)
            {
                auto padIndex = stoi(label.substr(4)) - 1;
                mpc.getActiveControls().lock()->pad(padIndex + (mpc.getBank() * 16), 127, false, 0);
            }
            else
            {
                hardwareComponent->push();
            }
        }
        else
        {
            hardwareComponent->release();
        }
    }
    else
    {
        // We have some key codes that map to themselves, not to a hardware component.
        if (label.compare("ctrl") == 0)
            mpc.getControls().lock()->setCtrlPressed(keyEvent.keyDown);
        else if (label.compare("alt") == 0)
            mpc.getControls().lock()->setAltPressed(keyEvent.keyDown);
        
        // And we have some things that are not buttons. Probably pads should be handled here
        // as well, so we can do some velocity calculation based on the current state of Mpc.
        else if (label.find("datawheel-") != string::npos && keyEvent.keyDown)
        {
            int increment = 1;
            
            if (mpc.getControls().lock()->isCtrlPressed())
                increment *= 10;
            
            if (mpc.getControls().lock()->isAltPressed())
                increment *= 10;
            
            if (mpc.getControls().lock()->isShiftPressed())
                increment *= 10;
            
            if (label.find("down") != string::npos)
                increment = -increment;
            
            mpc.getHardware().lock()->getDataWheel().lock()->turn(increment);
        }
    }
}
