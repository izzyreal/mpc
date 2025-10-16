#include "VmpcMidiControlMode.hpp"

#include "hardware/ComponentId.h"
#include "hardware/Hardware.h"
#include "hardware/HardwareComponent.h"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "nvram/MidiControlPersistence.hpp"

#include <engine/midi/ShortMessage.hpp>
#include <memory>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::midi;

void VmpcMidiControlMode::processMidiInputEvent(mpc::Mpc& mpc, mpc::engine::midi::ShortMessage* msg)
{
    auto status = msg->getStatus();
    auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;
    auto isNoteOn = status >= ShortMessage::NOTE_ON && status < ShortMessage::NOTE_ON + 16;
    auto isNoteOff = status >= ShortMessage::NOTE_OFF && status < ShortMessage::NOTE_OFF + 16;

    auto isChannelPressure = msg->isChannelPressure();

    const auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
    const auto hardware = mpc.getHardware();

    if (isChannelPressure)
    {
        if (const auto newPressure = msg->getData1(); newPressure > 0)
        {
            for (auto& p : hardware->getPads())
            {
                if (!p->isPressed()) continue;
                p->aftertouch(newPressure);
            }
        }

        return;
    }

    if (!isNoteOn && !isNoteOff && !isControl)
    {
        return;
    }

    if (vmpcMidiScreen->isLearning())
    {
        if (!isNoteOn && !isControl)
        {
            return;
        }

        vmpcMidiScreen->setLearnCandidate(isNoteOn, msg->getChannel(), msg->getData1(), msg->getData2());
        return;
    }

    auto dataWheel = hardware->getDataWheel();

    for (auto& labelCommand : vmpcMidiScreen->activePreset->rows)
    {
        const auto channelIndex = labelCommand.getMidiChannelIndex();

        if (channelIndex >= 0 && msg->getChannel() != channelIndex)
        {
            continue;
        }

        const auto commandNumber = labelCommand.getNumber();

        if (commandNumber != -1 && msg->getData1() != commandNumber)
        {
            continue;
        }

        const auto controllerValue = msg->getData2();
        const auto label = labelCommand.getMpcHardwareLabel();

        if (isControl)
        {
            if (!labelCommand.isCC())
            {
                continue;
            }

            if (labelCommand.getValue() != -1 &&
                labelCommand.getValue() != controllerValue)
            {
                continue;
            }
        }

        if ((isNoteOn || isNoteOff) && !labelCommand.isNote())
        {
            continue;
        }

        if (label.substr(0, 4) == "pad-")
        {
            assert(label.length() == 5 || label.length() == 6);
            const auto digitsString = label.substr(4);
            const auto padNumber = std::stoi(digitsString);
            auto mpcPad = mpc.getHardware()->getPad(padNumber - 1);

            if (msg->getData2() > 0 && !isNoteOff)
            {
                mpcPad->pressWithVelocity(msg->getData2());
            }
            else
            {
                mpcPad->release();
            }
        }

        if (label == "datawheel")
        {
            if (previousDataWheelValue == -1)
            {
                previousDataWheelValue = controllerValue;
            }

            if (previousDataWheelValue == 0 && controllerValue == 0)
            {
                dataWheel->turn(-1);
            }
            else if (previousDataWheelValue == 127 && controllerValue == 127)
            {
                dataWheel->turn(1);
            }
            else if (controllerValue - previousDataWheelValue != 0)
            {
                dataWheel->turn(controllerValue - previousDataWheelValue);
            }
            previousDataWheelValue = controllerValue;
        }
        else if (label == "datawheel-up")
        {
            dataWheel->turn(1);
        }
        else if (label == "datawheel-down")
        {
            dataWheel->turn(-1);
        }
        else if (label == "slider")
        {
            hardware->getSlider()->setValue(controllerValue);
        }
        else if (label == "rec-gain")
        {
            auto normalized = static_cast<unsigned char>(controllerValue / 1.27f);
            hardware->getRecPot()->setValue(normalized);
        }
        else if (label == "main-volume")
        {
            auto normalized = static_cast<unsigned char>(controllerValue / 1.27f);
            hardware->getVolPot()->setValue(normalized);
        }
        else if (auto pressable = std::dynamic_pointer_cast<hardware::Pressable>(hardware->getComponent(hardware::componentLabelToId.at(label))))
        {
            if (msg->getData2() == 0)
            {
                pressable->release();
            }
            else
            {
                if (auto withVelo = std::dynamic_pointer_cast<mpc::hardware::VelocitySensitivePressable>(pressable))
                {
                    withVelo->pressWithVelocity(msg->getData2());
                }
                else
                {
                    pressable->press();
                }
            }
        }
    }
}

