#pragma once

#include "IntTypes.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

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

        void setTarget(const std::string &n);

        void setMessageType(const std::string &t);

        void setMidiNumber(int n);

        void setMidiValue(int v);

        void setMidiChannelIndex(int ch);

        void setEnabled(bool e);

        const std::string &getTarget() const;

        const std::string &getMessageType() const;

        int getMidiNumber() const;

        int getMidiValue() const;

        int getMidiChannelIndex() const;

        bool isEnabled() const;
    };

    void to_json(json &j, const Binding &b);

    void from_json(const json &j, Binding &b);

    struct MidiControlPresetV3
    {
        ConstrainedInt<long long, 0, 4503599627370496LL> version{
            CURRENT_PRESET_VERSION};
        std::string name;
        std::string midiControllerDeviceName;
        std::string autoLoad{"No"};
        std::vector<Binding> bindings;

        void setVersion(long long v);

        void setName(const std::string &n);

        void setMidiControllerDeviceName(const std::string &n);

        void setAutoLoad(const std::string &a);

        void setBindings(const std::vector<Binding> &b);

        long long getVersion() const;

        const std::string &getName() const;

        const std::string &getMidiControllerDeviceName() const;

        const std::string &getAutoLoad() const;

        const std::vector<Binding> &getBindings() const;
    };

    void to_json(json &j, const MidiControlPresetV3 &p);

    void from_json(const json &j, MidiControlPresetV3 &p);

} // namespace mpc::input::midi
