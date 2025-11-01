#include "controller/ClientMidiFootswitchAssignmentController.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::midi::input;

ClientMidiFootswitchAssignmentController::ClientMidiFootswitchAssignmentController(
    std::shared_ptr<ClientHardwareEventController> ch,
    std::shared_ptr<lcdgui::screens::MidiSwScreen> ms,
    std::shared_ptr<sequencer::Sequencer> seq)
    : clientHardwareEventController(ch), midiSwScreen(ms), sequencer(seq)
{
}

void ClientMidiFootswitchAssignmentController::pressButton(ComponentId id)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::ButtonPress;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::releaseButton(ComponentId id)
{
    ClientHardwareEvent ev;
    ev.source = ClientHardwareEvent::Source::Internal;
    ev.componentId = id;
    ev.type = ClientHardwareEvent::Type::ButtonRelease;
    clientHardwareEventController->handleClientHardwareEvent(ev);
}

void ClientMidiFootswitchAssignmentController::triggerDualButtonCombo(ComponentId first, ComponentId second)
{
    pressButton(first);
    pressButton(second);
    releaseButton(second);
    releaseButton(first);
}

void ClientMidiFootswitchAssignmentController::handleStopToPlay() {
    pressButton(ComponentId::PLAY);
    releaseButton(ComponentId::PLAY);
}

void ClientMidiFootswitchAssignmentController::handleRecordingToPlay() {
    pressButton(ComponentId::REC);
    releaseButton(ComponentId::REC);
}

void ClientMidiFootswitchAssignmentController::handleRecPunch() {
    if (!sequencer->isPlaying()) handleStopToPlay();
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing()) {
        pressButton(ComponentId::REC);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::REC);
    } else if (sequencer->isRecording()) handleRecordingToPlay();
}

void ClientMidiFootswitchAssignmentController::handleOdubPunch() {
    if (!sequencer->isPlaying()) handleStopToPlay();
    else if (sequencer->isPlaying() && !sequencer->isRecordingOrOverdubbing()) {
        pressButton(ComponentId::OVERDUB);
        pressButton(ComponentId::PLAY);
        releaseButton(ComponentId::PLAY);
        releaseButton(ComponentId::OVERDUB);
    } else if (sequencer->isOverdubbing()) handleRecordingToPlay();
}

void ClientMidiFootswitchAssignmentController::dispatchSequencerCommand(
    MidiControlTarget::SequencerTarget::Command cmd)
{
    using Cmd = MidiControlTarget::SequencerTarget::Command;
    switch (cmd) {
        case Cmd::PLAY: handleStopToPlay(); break;
        case Cmd::REC: handleRecordingToPlay(); break;
        case Cmd::STOP: pressButton(ComponentId::STOP); releaseButton(ComponentId::STOP); break;
        case Cmd::REC_PLUS_PLAY: triggerDualButtonCombo(ComponentId::REC, ComponentId::PLAY); break;
        case Cmd::ODUB_PLUS_PLAY: triggerDualButtonCombo(ComponentId::OVERDUB, ComponentId::PLAY); break;
        case Cmd::REC_PUNCH: handleRecPunch(); break;
        case Cmd::ODUB_PUNCH: handleOdubPunch(); break;
    }
}

void ClientMidiFootswitchAssignmentController::handleEvent(const ClientMidiEvent &e)
{
    if (bindings.empty())
    {
        using Cmd = MidiControlTarget::SequencerTarget::Command;
        using MT  = MidiBindingBase::MessageType;

        for (auto [switchCC, functionIndex] : midiSwScreen->getSwitches()) {
            const std::string fn = midiSwScreen->getFunctionNames()[functionIndex];

            auto trim = [](std::string s) {
                s.erase(0, s.find_first_not_of(" \t"));
                s.erase(s.find_last_not_of(" \t") + 1);
                return s;
            };
            const std::string name = trim(fn);

            // PAD X
            if (name.rfind("PAD", 0) == 0) {
                std::string rest = trim(name.substr(3));
                const int padNum = std::stoi(rest);
                const auto id = static_cast<ComponentId>(
                    static_cast<int>(ComponentId::PAD_1_OR_AB) + padNum - 1);
                bindings.push_back(HardwareBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {id}});
                continue;
            }

            // Regular component IDs
            auto add = [&, switchCC = switchCC](ComponentId cid) {
                bindings.push_back(HardwareBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {cid}});
            };

            if (name == "PLAY STRT") add(ComponentId::PLAY_START);
            else if (name == "PLAY") add(ComponentId::PLAY);
            else if (name == "STOP") add(ComponentId::STOP);
            else if (name == "TAP") add(ComponentId::TAP_TEMPO_OR_NOTE_REPEAT);
            else if (name == "PAD BNK A") add(ComponentId::BANK_A);
            else if (name == "PAD BNK B") add(ComponentId::BANK_B);
            else if (name == "PAD BNK C") add(ComponentId::BANK_C);
            else if (name == "PAD BNK D") add(ComponentId::BANK_D);
            else if (name.rfind("F", 0) == 0 || name.find("   F") != std::string::npos) {
                const int fNum = std::stoi(name.substr(name.find('F') + 1));
                add(static_cast<ComponentId>(static_cast<int>(ComponentId::F1) + fNum - 1));
            }
            else if (name == "REC+PLAY")
                bindings.push_back(SequencerBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {Cmd::REC_PLUS_PLAY}});
            else if (name == "ODUB+PLAY")
                bindings.push_back(SequencerBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {Cmd::ODUB_PLUS_PLAY}});
            else if (name == "REC/PUNCH")
                bindings.push_back(SequencerBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {Cmd::REC_PUNCH}});
            else if (name == "ODUB/PNCH")
                bindings.push_back(SequencerBinding{{-1, switchCC, std::nullopt, Interaction::Press, MT::CC}, {Cmd::ODUB_PUNCH}});
        }
    }

    std::cout << "[MIDI] handleEvent called\n";
    e.printInfo();

    if (e.getMessageType() != ClientMidiEvent::MessageType::CONTROLLER) {
        std::cout << "[MIDI] Not a controller message, ignoring\n";
        return;
    }

    //int channel = e.getChannel();
    int number  = e.getControllerNumber();
    int value   = e.getControllerValue();
    bool pressed = value >= 64;

    std::cout /*<< "[MIDI] Channel=" << channel*/
              << " CC=" << number
              << " Value=" << value
              << " Pressed=" << pressed << "\n";

    int checked = 0;
    int matched = 0;

for (auto &binding : bindings) {
    ++checked;
    std::visit([&](auto &b) {
        std::cout << "  [Binding " << checked << "] "
                  << "ch=" << b.channel
                  << " num=" << b.number
                  << " interaction=" << static_cast<int>(b.interaction)
                  << "\n";

        if (b.number != number)
            return;

        ++matched;
        std::cout << "    → Match found. Pressed=" << pressed << "\n";

        if constexpr (std::is_same_v<std::decay_t<decltype(b)>, HardwareBinding>) {
            if (b.interaction == Interaction::Press) {
                if (pressed) {
                    std::cout << "    → ButtonPress for " << static_cast<int>(b.target.componentId) << "\n";
                    pressButton(b.target.componentId);
                } else {
                    std::cout << "    → ButtonRelease for " << static_cast<int>(b.target.componentId) << "\n";
                    releaseButton(b.target.componentId);
                }
            }
        } else if constexpr (std::is_same_v<std::decay_t<decltype(b)>, SequencerBinding>) {
            if (pressed && b.interaction == Interaction::Press) {
                std::cout << "    → Dispatching SequencerBinding command\n";
                dispatchSequencerCommand(b.target.command);
            }
        }
    }, binding);
}
    std::cout << "[MIDI] Checked " << checked << " bindings, matched " << matched << "\n";
}
