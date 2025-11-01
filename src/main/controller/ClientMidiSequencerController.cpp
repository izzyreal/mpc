#include "controller/ClientMidiSequencerController.hpp"

using namespace mpc::controller;
using namespace mpc::client::event;

ClientMidiSequencerController::ClientMidiSequencerController() = default;

void ClientMidiSequencerController::handleEvent(const ClientMidiEvent &e)
{
    printEventDebug(e);

    // Filter channel messages by mode
    if (!isOmniOn() && !isChannelMatch(e))
    {
        return;
    }

    switch (e.getMessageType())
    {
        // --- Channel Voice Messages ---
        case MessageType::NOTE_ON:
            handleNoteOn(e);
            break;
        case MessageType::NOTE_OFF:
            handleNoteOff(e);
            break;
        case MessageType::AFTERTOUCH:
            handleKeyAftertouch(e);
            break;
        case MessageType::CHANNEL_PRESSURE:
            handleChannelAftertouch(e);
            break;
        case MessageType::PITCH_WHEEL:
            handlePitchBend(e);
            break;
        case MessageType::CONTROLLER:
            handleControlChange(e);
            break;
        case MessageType::PROGRAM_CHANGE:
            handleProgramChange(e);
            break;

        // --- System Real-Time ---
        case MessageType::MIDI_CLOCK:
            handleMidiClock(e);
            break;
        case MessageType::MIDI_START:
            handleStart(e);
            break;
        case MessageType::MIDI_STOP:
            handleStop(e);
            break;
        case MessageType::MIDI_CONTINUE:
            handleContinue(e);
            break;

        // --- Future system messages (if extended) ---
        default:
            break;
    }
}

// ===================================================
// === Message Handlers ==============================
// ===================================================

void ClientMidiSequencerController::handleNoteOn(const ClientMidiEvent &e)
{
    const int ch = e.getChannel();
    const int note = e.getNoteNumber();
    const int vel = e.getVelocity();

    if (vel == 0)
    {
        handleNoteOff(e);
        return;
    }

    if (!isPolyMode())
    {
        enforceMonoMode(ch, note);
    }

    if (convertSustainPedalToDuration && sustainPedalState[ch])
    {
        holdNoteForSustain(ch, note);
    }

    noteOnInternal(ch, note, vel);
}

void ClientMidiSequencerController::handleNoteOff(const ClientMidiEvent &e)
{
    const int ch = e.getChannel();
    const int note = e.getNoteNumber();

    if (convertSustainPedalToDuration && sustainPedalState[ch])
    {
        holdNoteForSustain(ch, note);
        return;
    }

    noteOffInternal(ch, note);
}

void ClientMidiSequencerController::handleKeyAftertouch(
    const ClientMidiEvent &e)
{
    // Key-specific aftertouch recognized
}

void ClientMidiSequencerController::handleChannelAftertouch(
    const ClientMidiEvent &e)
{
    // Channel pressure recognized
}

void ClientMidiSequencerController::handlePitchBend(const ClientMidiEvent &e)
{
    // Pitch bend recognized (value range: -8192..8191)
}

void ClientMidiSequencerController::handleControlChange(
    const ClientMidiEvent &e)
{
    const int ch = e.getChannel();
    const int cc = e.getControllerNumber();
    const int val = e.getControllerValue();

    // Recognized controllers 0–127
    if (cc == 64) // Sustain Pedal
    {
        if (convertSustainPedalToDuration)
        {
            sustainPedalState[ch] = (val >= 64);
            if (!sustainPedalState[ch])
            {
                releaseSustainedNotes(ch);
            }
        }
        else
        {
            // Record sustain pedal message as-is
        }
    }
}

void ClientMidiSequencerController::handleProgramChange(
    const ClientMidiEvent &e)
{
    // Program Change (True Number 0–127)
}

void ClientMidiSequencerController::handleSystemExclusive(
    const ClientMidiEvent &e)
{
    // SysEx recognized only if ID == 0x45
}

void ClientMidiSequencerController::handleSongPositionPointer(
    const ClientMidiEvent &e)
{
    // Recognized System Common message
}

void ClientMidiSequencerController::handleSongSelect(const ClientMidiEvent &e)
{
    // Ignored by MPC2000XL
}

void ClientMidiSequencerController::handleTuneRequest(const ClientMidiEvent &e)
{
    // Ignored by MPC2000XL
}

void ClientMidiSequencerController::handleMidiClock(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiSequencerController::handleStart(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiSequencerController::handleStop(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiSequencerController::handleContinue(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiSequencerController::handleLocalOnOff(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiSequencerController::handleAllNotesOff(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiSequencerController::handleActiveSense(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiSequencerController::handleReset(const ClientMidiEvent &e)
{
    // Not recognized
}

// ===================================================
// === Helpers =======================================
// ===================================================

bool ClientMidiSequencerController::isChannelMatch(
    const ClientMidiEvent &e) const noexcept
{
    return (e.getChannel() + 1) == basicChannel;
}

bool ClientMidiSequencerController::isOmniOn() const noexcept
{
    return mode == 1 || mode == 2;
}

bool ClientMidiSequencerController::isPolyMode() const noexcept
{
    return mode == 1 || mode == 3;
}

void ClientMidiSequencerController::noteOnInternal(int channel, int note,
                                                   int velocity)
{
    heldNotes[channel].push_back(note);
}

void ClientMidiSequencerController::noteOffInternal(int channel, int note)
{
    auto &notes = heldNotes[channel];
    notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());
}

void ClientMidiSequencerController::holdNoteForSustain(int channel, int note)
{
    sustainedNotes[channel].push_back(note);
}

void ClientMidiSequencerController::releaseSustainedNotes(int channel)
{
    for (int note : sustainedNotes[channel])
    {
        noteOffInternal(channel, note);
    }
    sustainedNotes[channel].clear();
}

void ClientMidiSequencerController::enforceMonoMode(int channel, int newNote)
{
    if (!heldNotes[channel].empty())
    {
        for (int note : heldNotes[channel])
        {
            noteOffInternal(channel, note);
        }
    }
}

void ClientMidiSequencerController::printEventDebug(
    const ClientMidiEvent &e) const
{
    std::cout << "[Sequencer] Handling event type " << e.getMessageType()
              << " on channel " << (e.getChannel() + 1)
              << " data1=" << e.getNoteNumber() << " data2=" << e.getVelocity()
              << std::endl;
}
