#pragma once

#include "hardware/ComponentId.hpp"
#include "input/midi/MidiControlPresetV3.hpp"

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <unordered_set>

using mpc::hardware::componentIdToLabel;
using nlohmann::json;

using namespace mpc::input::midi;

inline void checkIRigPadsPreset(const json &preset)
{
    if (preset[nameKey] != "iRig PADS")
    {
        throw std::runtime_error("Preset name is '" +
                                 preset[nameKey].get<std::string>() +
                                 "' and not 'iRig PADS'");
    }
    if (preset[midiControllerDeviceNameKey] != "iRig PADS")
    {
        throw std::runtime_error("Device name is not 'iRig PADS'");
    }

    static constexpr int AllChannels = -1;
    static constexpr int DefaultMidiNumber = 0;
    static constexpr int DisabledMidiNumber = -1;
    static constexpr int DefaultMidiValue = 64;

    struct BindingSpec
    {
        std::string type;
        int number;
        std::optional<int> value;
        int channel;
    };

    static const auto NoteStr = mpc::input::midi::noteStr;
    static const auto CcStr = mpc::input::midi::controllerStr;

    using Id = mpc::hardware::ComponentId;

    auto getTarget = [&](const Id id) -> std::string
    {
        return "hardware:" + componentIdToLabel.at(id);
    };

    auto getDisabledBinding =
        [&](const Id id) -> std::pair<std::string, BindingSpec>
    {
        return {getTarget(id), BindingSpec{CcStr, DisabledMidiNumber,
                                           DefaultMidiValue, AllChannels}};
    };

    const std::unordered_map<std::string, BindingSpec> expected = {
        {getTarget(Id::PAD_1_OR_AB), {NoteStr, 35, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_2_OR_CD), {NoteStr, 36, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_3_OR_EF), {NoteStr, 38, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_4_OR_GH), {NoteStr, 40, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_5_OR_IJ), {NoteStr, 37, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_6_OR_KL), {NoteStr, 39, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_7_OR_MN), {NoteStr, 42, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_8_OR_OP), {NoteStr, 44, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_9_OR_QR), {NoteStr, 50, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_10_OR_ST), {NoteStr, 45, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_11_OR_UV), {NoteStr, 41, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_12_OR_WX), {NoteStr, 46, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_13_OR_YZ), {NoteStr, 51, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_14_OR_AMPERSAND_OCTOTHORPE),
         {NoteStr, 53, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_15_OR_HYPHEN_EXCLAMATION_MARK),
         {NoteStr, 49, std::nullopt, AllChannels}},
        {getTarget(Id::PAD_16_OR_PARENTHESES),
         {NoteStr, 52, std::nullopt, AllChannels}},
        {getTarget(Id::DATA_WHEEL) + ":negative",
         {CcStr, DisabledMidiNumber, DefaultMidiValue, AllChannels}},
        {getTarget(Id::DATA_WHEEL) + ":positive",
         {CcStr, DisabledMidiNumber, DefaultMidiValue, AllChannels}},
        {getTarget(Id::DATA_WHEEL), {CcStr, 7, DefaultMidiValue, AllChannels}},
        {getTarget(Id::SLIDER), {CcStr, 1, DefaultMidiValue, AllChannels}},
        {getTarget(Id::REC_GAIN_POT),
         {CcStr, 11, DefaultMidiValue, AllChannels}},
        {getTarget(Id::MAIN_VOLUME_POT),
         {CcStr, 10, DefaultMidiValue, AllChannels}},
        {getTarget(Id::FULL_LEVEL_OR_CASE_SWITCH),
         {CcStr, 20, DefaultMidiValue, AllChannels}},
        {getTarget(Id::SIXTEEN_LEVELS_OR_SPACE),
         {CcStr, 21, DefaultMidiValue, AllChannels}},
        getDisabledBinding(Id::CURSOR_LEFT_OR_DIGIT),
        getDisabledBinding(Id::CURSOR_RIGHT_OR_DIGIT),
        getDisabledBinding(Id::CURSOR_UP),
        getDisabledBinding(Id::CURSOR_DOWN),
        getDisabledBinding(Id::REC),
        getDisabledBinding(Id::OVERDUB),
        getDisabledBinding(Id::STOP),
        getDisabledBinding(Id::PLAY),
        getDisabledBinding(Id::PLAY_START),
        getDisabledBinding(Id::MAIN_SCREEN),
        getDisabledBinding(Id::PREV_STEP_OR_EVENT),
        getDisabledBinding(Id::NEXT_STEP_OR_EVENT),
        getDisabledBinding(Id::GO_TO),
        getDisabledBinding(Id::PREV_BAR_OR_START),
        getDisabledBinding(Id::NEXT_BAR_OR_END),
        getDisabledBinding(Id::TAP_TEMPO_OR_NOTE_REPEAT),
        getDisabledBinding(Id::NEXT_SEQ),
        getDisabledBinding(Id::TRACK_MUTE),
        getDisabledBinding(Id::OPEN_WINDOW),
        getDisabledBinding(Id::F1),
        getDisabledBinding(Id::F2),
        getDisabledBinding(Id::F3),
        getDisabledBinding(Id::F4),
        getDisabledBinding(Id::F5),
        getDisabledBinding(Id::F6),
        getDisabledBinding(Id::SHIFT),
        getDisabledBinding(Id::ENTER_OR_SAVE),
        getDisabledBinding(Id::UNDO_SEQ),
        getDisabledBinding(Id::ERASE),
        getDisabledBinding(Id::AFTER_OR_ASSIGN),
        getDisabledBinding(Id::BANK_A),
        getDisabledBinding(Id::BANK_B),
        getDisabledBinding(Id::BANK_C),
        getDisabledBinding(Id::BANK_D),
        getDisabledBinding(Id::NUM_0_OR_VMPC),
        getDisabledBinding(Id::NUM_1_OR_SONG),
        getDisabledBinding(Id::NUM_2_OR_MISC),
        getDisabledBinding(Id::NUM_3_OR_LOAD),
        getDisabledBinding(Id::NUM_4_OR_SAMPLE),
        getDisabledBinding(Id::NUM_5_OR_TRIM),
        getDisabledBinding(Id::NUM_6_OR_PROGRAM),
        getDisabledBinding(Id::NUM_7_OR_MIXER),
        getDisabledBinding(Id::NUM_8_OR_OTHER),
        getDisabledBinding(Id::NUM_9_OR_MIDI_SYNC)};

    std::vector<std::pair<std::string, json>> actual;
    std::unordered_map<std::string, int> seen;

    for (const auto &binding : preset[bindingsKey])
    {
        const std::string target = binding[targetKey].get<std::string>();
        int &count = seen[target];
        count++;
        if (count > 1)
        {
            throw std::runtime_error("Duplicate binding for target '" + target +
                                     "': occurrence " + std::to_string(count));
        }
        actual.emplace_back(target, binding);
    }

    std::unordered_set<std::string> matched;

    for (const auto &kv : expected)
    {
        const std::string &target = kv.first;
        const BindingSpec &spec = kv.second;
        bool found = false;

        for (auto &p : actual)
        {
            if (p.first != target)
            {
                continue;
            }

            const json &b = p.second;
            if (b[messageTypeKey] != spec.type)
            {
                continue;
            }
            if (b[midiNumberKey].get<int>() != spec.number)
            {
                continue;
            }
            if (b[midiChannelIndexKey].get<int>() != spec.channel)
            {
                continue;
            }

            if (spec.type == CcStr)
            {
                if (b.contains(midiValueKey) &&
                    spec.value != b[midiValueKey].get<int>())
                {
                    continue;
                }
            }
            else if (b.contains(midiValueKey))
            {
                continue;
            }

            found = true;
            matched.insert(target);
            break;
        }

        if (!found)
        {
            throw std::runtime_error(
                "Missing binding: " + target + " with midiValue " +
                std::to_string(kv.second.value.value_or(-2)) +
                " with midiNumber " + std::to_string(kv.second.number));
        }
    }

    for (const auto &[target, binding] : actual)
    {
        if (!expected.count(target))
        {
            throw std::runtime_error("Unexpected binding: " + target);
        }
    }
}
