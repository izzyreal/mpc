#pragma once

#include "IntTypes.hpp"
#include "input/Direction.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <optional>

namespace mpc::input::midi
{
    inline constexpr long long CURRENT_PRESET_VERSION = 3;

    using json = nlohmann::json;

    static const std::string hardwareStr = "hardware:";
    static const std::string sequencerStr = "sequencer:";
    static const std::string negativeStr = ":negative";
    static const std::string positiveStr = ":positive";
    static const std::string controllerStr = "controller";
    static const std::string controllerDisplayStr = "CC";
    static const std::string noteStr = "note";
    static const std::string noteDisplayStr = "Note";

    enum class BindingMessageType
    {
        Controller,
        Note
    };

    enum class BindingEncoderMode
    {
        RelativeStateless,
        RelativeStateful
    };

    inline std::string messageTypeToString(const BindingMessageType t)
    {
        if (t == BindingMessageType::Controller)
        {
            return controllerStr;
        }
        return noteStr;
    }

    inline std::string messageTypeToDisplayString(const BindingMessageType t)
    {
        if (t == BindingMessageType::Controller)
        {
            return controllerDisplayStr;
        }
        return noteDisplayStr;
    }

    inline BindingMessageType stringToMessageType(const std::string &s)
    {
        if (s == controllerStr)
        {
            return BindingMessageType::Controller;
        }

        if (s == noteStr)
        {
            return BindingMessageType::Note;
        }

        throw std::invalid_argument(
            "Message type string has to be 'controller' or 'note', but it was "
            "'" +
            s + "'");
    }

    inline std::string encoderModeToString(const BindingEncoderMode m)
    {
        switch (m)
        {
            case BindingEncoderMode::RelativeStateful:
                return "relative_stateful";
            case BindingEncoderMode::RelativeStateless:
                return "relative_stateless";
        }

        throw std::invalid_argument("Invalid encoderMode");
    }

    inline std::string encoderModeToDisplayString(const BindingEncoderMode m)
    {
        switch (m)
        {
            case BindingEncoderMode::RelativeStateful:
                return "RSF";
            case BindingEncoderMode::RelativeStateless:
                return "RSL";
        }

        throw std::invalid_argument("Invalid encoderMode");
    }

    inline BindingEncoderMode stringToEncoderMode(const std::string &s)
    {
        if (s == "relative_stateful")
        {
            return BindingEncoderMode::RelativeStateful;
        }
        if (s == "relative_stateless")
        {
            return BindingEncoderMode::RelativeStateless;
        }
        throw std::invalid_argument("Invalid encoderMode string: " + s);
    }

    struct Binding
    {
        std::string target;
        BindingMessageType messageType{BindingMessageType::Controller};
        BindingEncoderMode encoderMode{BindingEncoderMode::RelativeStateless};
        MidiNumber midiNumber{0};
        MidiValue midiValue{0};
        MidiChannel midiChannelIndex{0};

        bool operator==(Binding &other)
        {
            return target == other.target && messageType == other.messageType &&
                   midiNumber == other.midiNumber &&
                   midiValue == other.midiValue &&
                   midiChannelIndex == other.midiChannelIndex &&
                   encoderMode == other.encoderMode;
        }

        bool operator!=(Binding &other)
        {
            return !(*this == other);
        }

        Binding() = default;

        void setTarget(const std::string &n);

        std::string getTargetDisplayName() const;

        void setMessageType(const BindingMessageType);

        void setMidiNumber(int n);

        void setMidiValue(int v);

        void setMidiChannelIndex(int ch);

        void setEncoderMode(const BindingEncoderMode);

        const std::string &getTarget() const;

        std::optional<std::string> getHardwareTarget() const;

        Direction getHardwareDirection() const;

        std::optional<std::string> getSequencerTarget() const;

        const BindingMessageType &getMessageType() const;

        int getMidiNumber() const;

        int getMidiValue() const;

        int getMidiChannelIndex() const;

        bool isEnabled() const;

        BindingEncoderMode getEncoderMode() const;

        bool isController() const;

        bool isNote() const;

        bool isButtonLike() const;

        bool isNonButtonLikeDataWheel() const;
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

        Binding &getBindingByIndex(int);
    };

    void to_json(json &j, const MidiControlPresetV3 &p);

    void from_json(const json &j, MidiControlPresetV3 &p);

} // namespace mpc::input::midi
