#pragma once

#include "client/event/ClientMidiEvent.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <optional>

namespace mpc::controller
{

class ClientMidiSequencerController
{
public:
    ClientMidiSequencerController();

    void handleEvent(const mpc::client::event::ClientMidiEvent &e);

private:
    using ClientMidiEvent = mpc::client::event::ClientMidiEvent;
    using MessageType = ClientMidiEvent::MessageType;

    // ===================================================
    // === Core Sequencer Configuration and State =========
    // ===================================================

    // Basic MIDI channel (1–16)
    int basicChannel = 1;

    // Mode: 1–4 (OMNI ON/OFF × POLY/MONO)
    int mode = 1;

    // Sustain pedal behavior
    bool convertSustainPedalToDuration = false;

    // Sustain pedal and note tracking
    std::unordered_map<int, bool> sustainPedalState;      // channel → pedal on/off
    std::unordered_map<int, std::vector<int>> heldNotes;  // channel → active notes
    std::unordered_map<int, std::vector<int>> sustainedNotes; // notes held by sustain

    // ===================================================
    // === Message Handlers ===============================
    // ===================================================

    // Channel Voice Messages
    void handleNoteOn(const ClientMidiEvent &e);
    void handleNoteOff(const ClientMidiEvent &e);
    void handleKeyAftertouch(const ClientMidiEvent &e);
    void handleChannelAftertouch(const ClientMidiEvent &e);
    void handlePitchBend(const ClientMidiEvent &e);
    void handleControlChange(const ClientMidiEvent &e);
    void handleProgramChange(const ClientMidiEvent &e);

    // System Exclusive (Manufacturer ID: 0x45)
    void handleSystemExclusive(const ClientMidiEvent &e);

    // System Common
    void handleSongPositionPointer(const ClientMidiEvent &e);
    void handleSongSelect(const ClientMidiEvent &e);
    void handleTuneRequest(const ClientMidiEvent &e);

    // System Real-Time
    void handleMidiClock(const ClientMidiEvent &e);
    void handleStart(const ClientMidiEvent &e);
    void handleStop(const ClientMidiEvent &e);
    void handleContinue(const ClientMidiEvent &e);

    // Auxiliary / Non-Recognized Messages
    void handleLocalOnOff(const ClientMidiEvent &e);
    void handleAllNotesOff(const ClientMidiEvent &e);
    void handleActiveSense(const ClientMidiEvent &e);
    void handleReset(const ClientMidiEvent &e);

    // ===================================================
    // === Helpers ========================================
    // ===================================================

    bool isChannelMatch(const ClientMidiEvent &e) const noexcept;
    bool isOmniOn() const noexcept;
    bool isPolyMode() const noexcept;

    void noteOnInternal(int channel, int note, int velocity);
    void noteOffInternal(int channel, int note);

    void holdNoteForSustain(int channel, int note);
    void releaseSustainedNotes(int channel);

    void enforceMonoMode(int channel, int newNote);

    void printEventDebug(const ClientMidiEvent &e) const;
};

} // namespace mpc::controller

