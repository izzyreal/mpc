#include "KeyEventHandler.hpp"

#include <Mpc.hpp>

#include "KeyEvent.hpp"

#include <controls/KbMapping.hpp>

#include <lcdgui/screens/VmpcKeyboardScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwComponent.hpp>
#include <hardware/DataWheel.hpp>

#include "KeyCodeHelper.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::hardware;

KeyEventHandler::KeyEventHandler(mpc::Mpc &mpc)
        : mpc(mpc)
{
}

void KeyEventHandler::handle(const KeyEvent &keyEvent)
{
    //MLOG("KeyEventHandler::handle keyEvent.rawKeyCode " + std::to_string(keyEvent.rawKeyCode) + std::string(keyEvent.keyDown ? " down" : " up"));

    auto screen = mpc.screens->get<VmpcKeyboardScreen>("vmpc-keyboard");
    auto kbMapping = mpc.getControls()->getKbMapping().lock();

    auto it = find(begin(pressed), end(pressed), keyEvent.rawKeyCode);

    const auto vmpcKeyCode = KeyCodeHelper::getVmpcFromPlatformKeyCode(keyEvent.rawKeyCode);
    const bool isCapsLock =  vmpcKeyCode == VmpcKeyCode::VMPC_KEY_CapsLock;

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

//            Uncomment to disable key repeats:
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

    auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

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
            if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_UpArrow)
            {
                screen->up();
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_DownArrow)
            {
                screen->down();
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F1)
            {
                screen->function(0);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F2)
            {
                screen->function(1);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F3)
            {
                screen->function(2);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F4)
            {
                screen->function(3);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F5)
            {
                screen->function(4);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_F6)
            {
                screen->function(5);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_Escape)
            {
                screen->mainScreen();
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_ANSI_Equal)
            {
                screen->turnWheel(1);
            }
            else if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_ANSI_Minus)
            {
                screen->turnWheel(-1);
            }
        }

        return;
    }

    const bool allowTypingOfNames = true;
    const auto label = kbMapping->getHardwareComponentLabelAssociatedWithKeycode(vmpcKeyCode);

    if (keyEvent.keyDown && currentScreenName == "name" && allowTypingOfNames)
    {
        // When entering a name it's nice if you can use the computer keyboard
        // to type. Below we select some keypresses that should be handled as
        // usual, with some assumptions about mapping. The rest gets passed to
        // NameScreen for further processing.
        if (label != "left" && label != "right" && label != "f2" && label != "f3" && label != "f4" &&
            label != "f5" && label != "datawheel-down" && label != "datawheel-up" && label != "main-screen")
        {
            const auto nameScreen = mpc.screens->get<NameScreen>("name");

            if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_Backspace)
            {
                nameScreen->backSpace();
                return;
            }

            if (vmpcKeyCode == VmpcKeyCode::VMPC_KEY_Delete)
            {
                //nameScreen->deleteCurrentChar();
                return;
            }
            
            const auto keyCodeChar = KeyCodeHelper::getCharForTypableVmpcKeyCode(vmpcKeyCode);

            if (keyCodeChar == 0)
            {
                return;
            }

            const auto charWithCasing = static_cast<char>(mpc.getControls()->isShiftPressed() ? toupper(keyCodeChar) : tolower(keyCodeChar));
            nameScreen->typeCharacter(charWithCasing);
        }
    }

    //MLOG("Label associated with key code: " + label);

    auto hardwareComponent = mpc.getHardware()->getComponentByLabel(label);

    if (hardwareComponent)
    {
        if (keyEvent.keyDown)
        {
            if ((label.length() == 5 || label.length() == 6) && label.find("pad-") != std::string::npos)
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
            mpc.getControls()->setCtrlPressed(keyEvent.keyDown);
        else if (label == "alt")
            mpc.getControls()->setAltPressed(keyEvent.keyDown);

        // And we have some things that are not buttons. Probably pads should be handled here
        // as well, so we can do some velocity calculation based on the current state of Mpc.
        else if (label.find("datawheel-") != std::string::npos && keyEvent.keyDown)
        {
            int increment = 1;

            if (mpc.getControls()->isCtrlPressed())
                increment *= 10;

            if (mpc.getControls()->isAltPressed())
                increment *= 10;

            if (mpc.getControls()->isShiftPressed())
                increment *= 10;

            if (label.find("down") != std::string::npos)
                increment = -increment;

            mpc.getHardware()->getDataWheel()->turn(increment);
        }
    }
}
