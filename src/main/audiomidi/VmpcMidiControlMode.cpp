#include "VmpcMidiControlMode.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwComponent.hpp"
#include "hardware/DataWheel.hpp"
#include "hardware/HwSlider.hpp"
#include "hardware/Pot.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include "midi/core/ShortMessage.hpp"

using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace ctoot::midi::core;

void VmpcMidiControlMode::processMidiInputEvent(mpc::Mpc& mpc, ctoot::midi::core::ShortMessage *msg)
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

    auto hardware = mpc.getHardware().lock();
    auto dataWheel = hardware->getDataWheel().lock();

    for (auto& labelCommand : vmpcMidiScreen->labelCommands)
    {
        auto command = labelCommand.second;
        auto channelIndex = command.channelIndex;

        if (channelIndex >= 0 && msg->getChannel() != channelIndex)
        {
            continue;
        }

        auto label = labelCommand.first;
        auto isNote = command.isNote;
        auto commandValue = command.value;

        if (msg->getData1() != commandValue)
        {
            continue;
        }

        auto controllerValue = msg->getData2();

        if ((isNote && (isNoteOn || isNoteOff)) ||
            (!isNote && isControl))
        {
            auto hwComponent = hardware->getComponentByLabel(label).lock();

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
            else if (label == "slider")
            {
                hardware->getSlider().lock()->setValue(127 - controllerValue);
            }
            else if (label == "rec-gain")
            {
                auto normalized = static_cast<unsigned char>(controllerValue / 1.27f);
                hardware->getRecPot().lock()->setValue(normalized);
            }
            else if (label == "main-volume")
            {
                auto normalized = static_cast<unsigned char>(controllerValue / 1.27f);
                hardware->getVolPot().lock()->setValue(normalized);
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
