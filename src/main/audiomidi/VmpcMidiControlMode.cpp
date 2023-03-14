#include "VmpcMidiControlMode.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwComponent.hpp"
#include "hardware/DataWheel.hpp"
#include "hardware/HwSlider.hpp"
#include "hardware/Pot.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include <engine/midi/ShortMessage.hpp>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::midi;

void VmpcMidiControlMode::processMidiInputEvent(mpc::Mpc& mpc, mpc::engine::midi::ShortMessage *msg)
{
    auto status = msg->getStatus();
    auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;
    auto isNoteOn = status >= ShortMessage::NOTE_ON && status < ShortMessage::NOTE_ON + 16;
    auto isNoteOff = status >= ShortMessage::NOTE_OFF && status < ShortMessage::NOTE_OFF + 16;

    if (!isNoteOn && !isNoteOff && !isControl)
    {
        return;
    }

    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    if (vmpcMidiScreen->isLearning())
    {
        if (!isNoteOn && !isControl)
        {
            return;
        }

        vmpcMidiScreen->setLearnCandidate(isNoteOn, msg->getChannel(), msg->getData1());
        return;
    }

    auto hardware = mpc.getHardware();
    auto dataWheel = hardware->getDataWheel();

    for (auto& labelCommand : vmpcMidiScreen->activePreset->rows)
    {
        auto channelIndex = labelCommand.channel;

        if (channelIndex >= 0 && msg->getChannel() != channelIndex)
        {
            continue;
        }

        auto label = labelCommand.label;
        auto isNote = labelCommand.isNote;
        auto commandValue = labelCommand.value;

        if (msg->getData1() != commandValue)
        {
            continue;
        }

        auto controllerValue = msg->getData2();

        if ((isNote && (isNoteOn || isNoteOff)) ||
            (!isNote && isControl))
        {
            auto hwComponent = hardware->getComponentByLabel(label);

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
            else if (msg->getData2() == 0)
            {
                hwComponent->release();
            }
            else
            {
                hwComponent->push(msg->getData2());
                hwComponent->push();
            }
        }
    }
}
