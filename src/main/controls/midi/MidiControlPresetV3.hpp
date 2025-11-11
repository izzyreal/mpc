#pragma once
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <set>
#include <nlohmann/json.hpp>
#include "ConstrainedInt.hpp"

namespace mpc::controls::midi
{

    inline constexpr long long CURRENT_PRESET_VERSION = 3;

    using json = nlohmann::json;

    struct Binding
    {
        std::string labelName;
        std::string messageType;
        ConstrainedInt<int, 0, 127> midiNumber{0};
        ConstrainedInt<int, -1, 127> midiValue{0};
        ConstrainedInt<int, -1, 15> midiChannelIndex{0};
        bool enabled{false};

        Binding() = default;

        // --- Setters with validation ---
        void setLabelName(const std::string &n)
        {
            static const std::regex pattern("^[a-z0-9_\\-#]+$");
            if (n.empty() || n.size() > 15 || !std::regex_match(n, pattern))
            {
                throw std::invalid_argument("Invalid labelName");
            }
            labelName = n;
        }

        void setMessageType(const std::string &t)
        {
            if (t != "CC" && t != "Note")
            {
                throw std::invalid_argument(
                    "messageType must be 'CC' or 'Note'");
            }
            messageType = t;
        }

        void setMidiNumber(int n)
        {
            midiNumber = {n};
        }
        void setMidiValue(int v)
        {
            midiValue = {v};
        }
        void setMidiChannelIndex(int ch)
        {
            midiChannelIndex = {ch};
        }
        void setEnabled(bool e)
        {
            enabled = e;
        }

        // --- Getters ---
        const std::string &getLabelName() const
        {
            return labelName;
        }
        const std::string &getMessageType() const
        {
            return messageType;
        }
        int getMidiNumber() const
        {
            return midiNumber;
        }
        int getMidiValue() const
        {
            return midiValue;
        }
        int getMidiChannelIndex() const
        {
            return midiChannelIndex;
        }
        bool isEnabled() const
        {
            return enabled;
        }
    };

    // --- JSON conversions for Binding ---
    inline void to_json(json &j, const Binding &b)
    {
        j = json{{"labelName", b.getLabelName()},
                 {"messageType", b.getMessageType()},
                 {"midiNumber", b.getMidiNumber()},
                 {"midiChannelIndex", b.getMidiChannelIndex()},
                 {"enabled", b.isEnabled()}};
        if (b.getMessageType() == "CC")
        {
            j["midiValue"] = b.getMidiValue();
        }
    }

    inline void from_json(const json &j, Binding &b)
    {
        std::string label, type;
        j.at("labelName").get_to(label);
        j.at("messageType").get_to(type);

        b.setLabelName(label);
        b.setMessageType(type);
        b.setMidiNumber(j.at("midiNumber").get<int>());
        b.setMidiChannelIndex(j.at("midiChannelIndex").get<int>());
        b.setEnabled(j.at("enabled").get<bool>());

        if (type == "CC")
        {
            if (!j.contains("midiValue"))
            {
                throw std::invalid_argument("CC binding requires midiValue");
            }
            b.setMidiValue(j.at("midiValue").get<int>());
        }
        else
        {
            if (j.contains("midiValue"))
            {
                throw std::invalid_argument(
                    "Note binding must not include midiValue");
            }
        }
    }

    struct MidiControlPresetV3
    {
        ConstrainedInt<long long, 0, 4503599627370496LL> version{
            CURRENT_PRESET_VERSION};
        std::string name;
        std::string midiControllerDeviceName;
        std::string autoLoad{"No"};
        std::vector<Binding> bindings;

        // --- Setters with schema validation ---
        void setVersion(long long v)
        {
            version = {v};
        }

        void setName(const std::string &n)
        {
            static const std::regex pattern(
                "^[A-Za-z0-9 !#\\$%&'\\(\\)\\-@_{}]+$");
            if (n.empty() || n.size() > 16 || !std::regex_match(n, pattern))
            {
                throw std::invalid_argument("Invalid name (must match schema)");
            }
            name = n;
        }

        void setMidiControllerDeviceName(const std::string &n)
        {
            static const std::regex pattern(
                "^[a-zA-Z0-9 _\\-\\.\\(\\)\\/\\,;]*$");
            if (n.size() > 255 || !std::regex_match(n, pattern))
            {
                throw std::invalid_argument("Invalid midiControllerDeviceName");
            }
            midiControllerDeviceName = n;
        }

        void setAutoLoad(const std::string &a)
        {
            if (a != "No" && a != "Ask" && a != "Yes")
            {
                throw std::invalid_argument(
                    "autoLoad must be one of: No, Ask, Yes");
            }
            autoLoad = a;
        }

        void setBindings(const std::vector<Binding> &b)
        {
            if (b.size() != 81)
            {
                throw std::invalid_argument(
                    "bindings must contain exactly 81 entries");
            }

            static const std::set<std::string> requiredLabels = {
                "left",
                "right",
                "up",
                "down",
                "rec",
                "overdub",
                "stop",
                "play",
                "play-start",
                "main-screen",
                "prev-step-event",
                "next-step-event",
                "go-to",
                "prev-bar-start",
                "next-bar-end",
                "tap",
                "next-seq",
                "track-mute",
                "open-window",
                "full-level",
                "sixteen-levels",
                "f1",
                "f2",
                "f3",
                "f4",
                "f5",
                "f6",
                "shift",
                "shift_#1",
                "shift_#2",
                "shift_#3",
                "enter",
                "undo-seq",
                "erase",
                "after",
                "bank-a",
                "bank-b",
                "bank-c",
                "bank-d",
                "0",
                "1",
                "2",
                "3",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "0_extra",
                "1_extra",
                "2_extra",
                "3_extra",
                "4_extra",
                "5_extra",
                "6_extra",
                "7_extra",
                "8_extra",
                "9_extra",
                "pad-1",
                "pad-2",
                "pad-3",
                "pad-4",
                "pad-5",
                "pad-6",
                "pad-7",
                "pad-8",
                "pad-9",
                "pad-10",
                "pad-11",
                "pad-12",
                "pad-13",
                "pad-14",
                "pad-15",
                "pad-16",
                "datawheel",
                "datawheel-up",
                "datawheel-down",
                "slider",
                "rec-gain",
                "main-volume"};

            std::set<std::string> labels;

            for (const auto &bind : b)
            {
                labels.insert(bind.labelName);
            }

            if (labels != requiredLabels)
            {
                throw std::invalid_argument(
                    "bindings must include exactly one of each required "
                    "labelName");
            }

            bindings = b;
        }

        // --- Getters ---
        long long getVersion() const
        {
            return version;
        }
        const std::string &getName() const
        {
            return name;
        }
        const std::string &getMidiControllerDeviceName() const
        {
            return midiControllerDeviceName;
        }
        const std::string &getAutoLoad() const
        {
            return autoLoad;
        }
        const std::vector<Binding> &getBindings() const
        {
            return bindings;
        }
    };

    // --- JSON conversions for MidiControlPreset ---
    inline void to_json(json &j, const MidiControlPresetV3 &p)
    {
        j = json{{"version", p.getVersion()},
                 {"name", p.getName()},
                 {"midiControllerDeviceName", p.getMidiControllerDeviceName()},
                 {"autoLoad", p.getAutoLoad()},
                 {"bindings", p.getBindings()}};
    }

    inline void from_json(const json &j, MidiControlPresetV3 &p)
    {
        // Enforce no additional properties
        static const std::set<std::string> allowedKeys = {
            "version", "name", "midiControllerDeviceName", "autoLoad",
            "bindings"};

        for (auto &[key, _] : j.items())
        {
            if (allowedKeys.count(key) == 0)
            {
                throw std::invalid_argument(
                    "Unknown key in MidiControlPresetV3 JSON: " + key);
            }
        }

        auto v = j.at("version").get<long long>();
        if (v > CURRENT_PRESET_VERSION)
        {
            throw std::runtime_error(
                "Preset version is newer than this application supports.");
        }

        p.setVersion(v);
        p.setName(j.at("name").get<std::string>());

        if (j.contains("midiControllerDeviceName"))
        {
            p.setMidiControllerDeviceName(
                j.at("midiControllerDeviceName").get<std::string>());
        }

        p.setAutoLoad(j.at("autoLoad").get<std::string>());
        p.setBindings(j.at("bindings").get<std::vector<Binding>>());
    }

} // namespace mpc::controls::midi
