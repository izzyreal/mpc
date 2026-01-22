#pragma once

#include "IntTypes.hpp"
#include "input/Direction.hpp"
#include "input/midi/AutoLoadMode.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <optional>

namespace mpc::input::midi
{
    inline constexpr long long CURRENT_PRESET_VERSION = 3;

    using json = nlohmann::json;

    static const std::string autoLoadModeKey = "autoLoadMode";
    static const std::string messageTypeKey = "messageType";
    static const std::string midiValueKey = "midiValue";
    static const std::string midiNumberKey = "midiNumber";
    static const std::string midiChannelIndexKey = "midiChannelIndex";
    static const std::string encoderModeKey = "encoderMode";
    static const std::string bindingsKey = "bindings";
    static const std::string nameKey = "name";
    static const std::string versionKey = "version";
    static const std::string targetKey = "target";
    static const std::string midiControllerDeviceNameKey =
        "midiControllerDeviceName";

    static const std::string hardwareStr = "hardware:";
    static const std::string negativeStr = ":negative";
    static const std::string positiveStr = ":positive";
    static const std::string controllerStr = "controller";
    static const std::string controllerDisplayStr = "CC";
    static const std::string noteStr = "note";
    static const std::string noteDisplayStr = "Note";
    static const std::string relativeStatefulStr = "relative_stateful";
    static const std::string relativeStatelessStr = "relative_stateless";
    static const std::string autoLoadModeYesStr = "Yes";
    static const std::string autoLoadModeNoStr = "No";
    static const std::string autoLoadModeAskStr = "Ask";

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
                return relativeStatefulStr;
            case BindingEncoderMode::RelativeStateless:
                return relativeStatelessStr;
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
        if (s == relativeStatefulStr)
        {
            return BindingEncoderMode::RelativeStateful;
        }
        if (s == relativeStatelessStr)
        {
            return BindingEncoderMode::RelativeStateless;
        }
        throw std::invalid_argument("Invalid encoderMode string: " + s);
    }

    inline std::string autoLoadModeToString(const AutoLoadMode m)
    {
        if (m == AutoLoadModeAsk)
        {
            return autoLoadModeAskStr;
        }
        if (m == AutoLoadModeYes)
        {
            return autoLoadModeYesStr;
        }
        if (m == AutoLoadModeNo)
        {
            return autoLoadModeNoStr;
        }
        throw std::invalid_argument("Invalid autoLoadMode");
    }

    inline AutoLoadMode stringToAutoLoadMode(const std::string &s)
    {
        if (s == autoLoadModeAskStr)
        {
            return AutoLoadModeAsk;
        }
        if (s == autoLoadModeYesStr)
        {
            return AutoLoadModeYes;
        }
        if (s == autoLoadModeNoStr)
        {
            return AutoLoadModeNo;
        }
        throw std::invalid_argument("Invalid autoLoadMode string: " + s);
    }

    struct Binding
    {
        std::string target;
        BindingMessageType messageType{BindingMessageType::Controller};
        BindingEncoderMode encoderMode{BindingEncoderMode::RelativeStateless};
        MidiNumber midiNumber{0};
        MidiValue midiValue{0};
        MidiChannel midiChannelIndex{0};

        bool operator==(const Binding &other) const
        {
            return target == other.target && messageType == other.messageType &&
                   midiNumber == other.midiNumber &&
                   midiValue == other.midiValue &&
                   midiChannelIndex == other.midiChannelIndex &&
                   encoderMode == other.encoderMode;
        }

        bool operator!=(const Binding &other) const
        {
            return !(*this == other);
        }

        Binding() = default;

        void setTarget(const std::string &n);

        std::string getTargetDisplayName() const;

        void setMessageType(BindingMessageType);

        void setMidiNumber(int n);

        void setMidiValue(int v);

        void setMidiChannelIndex(int ch);

        void setEncoderMode(BindingEncoderMode);

        const std::string &getTarget() const;

        std::optional<std::string> getHardwareTarget() const;

        Direction getHardwareDirection() const;

        const BindingMessageType &getMessageType() const;

        int getMidiNumber() const;

        int getMidiValue() const;

        int getMidiChannelIndex() const;

        bool isEnabled() const;

        BindingEncoderMode getEncoderMode() const;

        bool isController() const;

        bool isNote() const;

        bool isButtonLike() const;

        bool isPad() const;

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
        AutoLoadMode autoLoadMode{AutoLoadModeAsk};
        std::vector<Binding> bindings;

        void setVersion(long long v);

        void setName(const std::string &n);

        void setMidiControllerDeviceName(const std::string &n);

        void setAutoLoadMode(AutoLoadMode);

        void setBindings(const std::vector<Binding> &b);

        long long getVersion() const;

        const std::string &getName() const;

        const std::string &getMidiControllerDeviceName() const;

        AutoLoadMode getAutoLoadMode() const;

        const std::vector<Binding> &getBindings() const;

        Binding &getBindingByIndex(int);

        bool hasBindingForNote(int) const;
    };

    void to_json(json &j, const MidiControlPresetV3 &p);

    void from_json(const json &j, MidiControlPresetV3 &p);

} // namespace mpc::input::midi
