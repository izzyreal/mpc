#include "KeyEventHandler.hpp"

#include <Mpc.hpp>

#include "KeyEvent.hpp"

#include <controls/KbMapping.hpp>

#include <lcdgui/screens/VmpcKeyboardScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwComponent.hpp>
#include <hardware/DataWheel.hpp>

#include <sys/KeyCodes.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace mpc::hardware;
using namespace moduru::sys;
using namespace std;

KeyEventHandler::KeyEventHandler(mpc::Mpc &mpc)
        : mpc(mpc)
{
}

void KeyEventHandler::handle(const KeyEvent &keyEvent)
{
    //MLOG("KeyEventHandler::handle keyEvent.rawKeyCode " + to_string(keyEvent.rawKeyCode) + string(keyEvent.keyDown ? " down" : " up"));

    auto screen = mpc.screens->get<VmpcKeyboardScreen>("vmpc-keyboard");
    auto kbMapping = mpc.getControls().lock()->getKbMapping().lock();

    auto it = find(begin(pressed), end(pressed), keyEvent.rawKeyCode);

    bool isCapsLock = KeyCodes::getKeyCodeName(keyEvent.rawKeyCode) == "caps lock";

    // Special case as caps lock only sends key releases on OSX
    if (isCapsLock)
    {
        if (it == end(pressed))
        {
            // TODO: We must fire a "push" to whatever control caps lock is bound to.
            pressed.push_back(keyEvent.rawKeyCode);
        }
        else
        {
            // TODO: We must fire a "release" to whatever control caps lock is bound to.
            pressed.erase(it);
        }
    }
    else
    {
        if (keyEvent.keyDown)
        {

//            Uncomment to disable key repeats
//            if (it != end(pressed))
//                return;

            pressed.push_back(keyEvent.rawKeyCode);

        }
        else
        {

            // There are some odd cases where a key down event actually comes in as a key up.
            // Concrete example on OSX: [fn + left shift] produces a key up event when pressed,
            // and another key up event when released.
            if (it != end(pressed))
                pressed.erase(it);
        }
    }

    auto currentScreenName = mpc.getLayeredScreen().lock()->getCurrentScreenName();
    auto keyCodeDisplayName = KeyCodes::getKeyCodeName(keyEvent.rawKeyCode);

    if (currentScreenName == "vmpc-keyboard")
    {
        if (!keyEvent.keyDown)
            return;

        if (screen->isLearning())
        {
            screen->setLearnCandidate(keyEvent.rawKeyCode);
        }
        else
        {
            if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_UpArrow])
            {
                screen->up();
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_DownArrow])
            {
                screen->down();
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F1])
            {
                screen->function(0);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F2])
            {
                screen->function(1);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F3])
            {
                screen->function(2);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F4])
            {
                screen->function(3);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F5])
            {
                screen->function(4);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_F6])
            {
                screen->function(5);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_Escape])
            {
                screen->mainScreen();
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_ANSI_Equal])
            {
                screen->turnWheel(1);
            }
            else if (keyCodeDisplayName == KeyCodes::names[KeyCodes::MVK_ANSI_Minus])
            {
                screen->turnWheel(-1);
            }
        }

        return;
    }

    bool allowTypingOfNames = true;
    auto label = kbMapping->getLabelFromKeyCode(keyEvent.rawKeyCode);

    if (keyEvent.keyDown && currentScreenName == "name" && allowTypingOfNames)
    {
        // When entering a name it's nice if you can use the computer keyboard
        // to type. Below we select some keypresses that should be handled as
        // usual, with some assumptions about mapping. The rest gets passed to
        // NameScreen for further processing.
        if (label != "left" && label != "right" && label != "f2" && label != "f3" && label != "f4" &&
            label != "f5" && label != "datawheel-down" && label != "datawheel-up" && label != "main-screen")
        {
            auto keyCodeString = kbMapping->getKeyCodeString(keyEvent.rawKeyCode);
            if (keyCodeString == "space") keyCodeString = " ";
            auto nameScreen = mpc.screens->get<mpc::lcdgui::screens::window::NameScreen>("name");
            if (keyCodeString.length() == 1)
            {
                nameScreen->typeCharacter(keyCodeString[0]);
                return;
            }

            if (keyCodeString == "backspace")
            {
                nameScreen->backSpace();
                return;
            }
        }
    }

    //MLOG("Label associated with key code: " + label);

    auto hardwareComponent = mpc.getHardware().lock()->getComponentByLabel(label).lock();

    if (hardwareComponent)
    {
        if (keyEvent.keyDown)
        {
            if ((label.length() == 5 || label.length() == 6) && label.find("pad-") != string::npos)
            {
                hardwareComponent->push(127);
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
        if (label == "ctrl")
            mpc.getControls().lock()->setCtrlPressed(keyEvent.keyDown);
        else if (label == "alt")
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
