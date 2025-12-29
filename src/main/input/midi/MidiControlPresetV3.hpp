#pragma once
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <set>
#include <nlohmann/json.hpp>
#include "IntTypes.hpp"

namespace mpc::input::midi
{
    inline constexpr long long CURRENT_PRESET_VERSION = 3;

    using json = nlohmann::json;

    struct Binding
    {
        std::string target;
        std::string messageType;
        MidiNumber midiNumber{0};
        MidiValue midiValue{0};
        MidiChannel midiChannelIndex{0};
        bool enabled{false};

        Binding() = default;

        void setTarget(const std::string &n)
        {
            static const std::regex pattern("^[a-z0-9_\\-#:]+$");
            if (n.empty() || n.size() > 64 || !std::regex_match(n, pattern))
            {
                throw std::invalid_argument("Invalid target");
            }
            target = n;
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
            midiNumber = MidiNumber(n);
        }
        void setMidiValue(int v)
        {
            midiValue = MidiValue(v);
        }
        void setMidiChannelIndex(int ch)
        {
            midiChannelIndex = MidiChannel(ch);
        }
        void setEnabled(bool e)
        {
            enabled = e;
        }

        const std::string &getTarget() const
        {
            return target;
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

    inline void to_json(json &j, const Binding &b)
    {
        j = json{{"target", b.getTarget()},
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
        std::string target, type;
        j.at("target").get_to(target);
        j.at("messageType").get_to(type);

        b.setTarget(target);
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

        void setVersion(long long v)
        {
            version = ConstrainedInt<long long, 0, 4503599627370496LL>{v};
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
            static const std::set<std::string> availableTargets = {
                "hardware:cursor-left-or-digit",
                "hardware:cursor-right-or-digit",
                "hardware:cursor-up",
                "hardware:cursor-down",
                "hardware:rec",
                "hardware:overdub",
                "hardware:stop",
                "hardware:play",
                "hardware:play-start",
                "hardware:main-screen",
                "hardware:prev-step-or-event",
                "hardware:next-step-or-event",
                "hardware:go-to",
                "hardware:prev-bar-or-start",
                "hardware:next-bar-or-end",
                "hardware:tap-or-note-repeat",
                "hardware:next-seq",
                "hardware:track-mute",
                "hardware:open-window",
                "hardware:full-level-or-case-switch",
                "hardware:sixteen-levels-or-space",
                "hardware:f1",
                "hardware:f2",
                "hardware:f3",
                "hardware:f4",
                "hardware:f5",
                "hardware:f6",
                "hardware:shift",
                "hardware:enter-or-save",
                "hardware:undo-seq",
                "hardware:erase",
                "hardware:after-or-assign",
                "hardware:bank-a",
                "hardware:bank-b",
                "hardware:bank-c",
                "hardware:bank-d",
                "hardware:0-or-vmpc",
                "hardware:1-or-song",
                "hardware:2-or-misc",
                "hardware:3-or-load",
                "hardware:4-or-sample",
                "hardware:5-or-trim",
                "hardware:6-or-program",
                "hardware:7-or-mixer",
                "hardware:8-or-other",
                "hardware:9-or-midi-sync",
                "hardware:pad-1-or-ab",
                "hardware:pad-2-or-cd",
                "hardware:pad-3-or-ef",
                "hardware:pad-4-or-gh",
                "hardware:pad-5-or-ij",
                "hardware:pad-6-or-kl",
                "hardware:pad-7-or-mn",
                "hardware:pad-8-or-op",
                "hardware:pad-9-or-qr",
                "hardware:pad-10-or-st",
                "hardware:pad-11-or-uv",
                "hardware:pad-12-or-wx",
                "hardware:pad-13-or-yz",
                "hardware:pad-14-or-ampersand-octothorpe",
                "hardware:pad-15-or-hyphen-eclamation-mark",
                "hardware:pad-16-or-parentheses",
                "hardware:datawheel:negative",
                "hardware:datawheel:positive",
                "hardware:slider",
                "hardware:rec-gain",
                "hardware:main-volume"
            };

            std::set<std::string> targets;

            for (const auto &bind : b)
            {
                if (std::find(availableTargets.begin(), availableTargets.end(),
                              bind.target) == availableTargets.end())
                {
                    throw std::invalid_argument("binding has unknown target '" +
                                                bind.target + "'");
                }

                targets.insert(bind.target);
            }

            bindings = b;
        }

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

} // namespace mpc::input::midi
