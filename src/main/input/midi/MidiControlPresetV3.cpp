#include "input/midi/MidiControlPresetV3.hpp"

#include "IntTypes.hpp"

#include "StrUtil.hpp"

#include "input/midi/MidiControlPresetUtil.hpp"
#include "hardware/ComponentId.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <set>

using namespace mpc::input::midi;

void Binding::setTarget(const std::string &n)
{
    static const std::regex pattern("^[a-z0-9_\\-#:]+$");
    if (n.empty() || n.size() > 64 || !std::regex_match(n, pattern))
    {
        throw std::invalid_argument("Invalid target");
    }
    target = n;
}

void Binding::setMessageType(const BindingMessageType t)
{
    messageType = t;
}

void Binding::setMidiNumber(int n)
{
    midiNumber = MidiNumber(n);
}

void Binding::setMidiValue(int v)
{
    midiValue = MidiValue(std::clamp(v, 1, 127));
}

void Binding::setMidiChannelIndex(int ch)
{
    midiChannelIndex = MidiChannel(ch);
}

void Binding::setEncoderMode(const BindingEncoderMode m)
{
    encoderMode = m;
}

const std::string &Binding::getTarget() const
{
    return target;
}

const BindingMessageType &Binding::getMessageType() const
{
    return messageType;
}

int Binding::getMidiNumber() const
{
    return midiNumber;
}

int Binding::getMidiValue() const
{
    return midiValue;
}

int Binding::getMidiChannelIndex() const
{
    return midiChannelIndex;
}

bool Binding::isEnabled() const
{
    return midiNumber != NoMidiNumber;
}

BindingEncoderMode Binding::getEncoderMode() const
{
    return encoderMode;
}

bool Binding::isController() const
{
    return getMessageType() == BindingMessageType::Controller;
}

bool Binding::isNote() const
{
    return getMessageType() == BindingMessageType::Note;
}

bool Binding::isButtonLike() const
{
    const auto hardwareTargetStr = getHardwareTarget();

    if (hardwareTargetStr)
    {
        const auto id = hardware::componentLabelToId.at(*hardwareTargetStr);

        if (hardware::isButtonId(id))
        {
            return true;
        }
    }

    if (getTarget() == "hardware:data-wheel:negative" ||
        getTarget() == "hardware:data-wheel:positive")
    {
        return true;
    }

    return false;
}

bool Binding::isNonButtonLikeDataWheel() const
{
    if (const auto hardwareTargetStr = getHardwareTarget())
    {
        return hardware::componentLabelToId.at(*hardwareTargetStr) ==
                   hardware::ComponentId::DATA_WHEEL &&
               getHardwareDirection() == Direction::NoDirection;
    }

    return false;
}

std::optional<std::string> Binding::getHardwareTarget() const
{
    if (target.find(hardwareStr) == std::string::npos)
    {
        return std::nullopt;
    }

    std::string result = target;

    StrUtil::replace(result, hardwareStr, "");
    StrUtil::replace(result, negativeStr, "");
    StrUtil::replace(result, positiveStr, "");

    return result;
}

mpc::input::Direction Binding::getHardwareDirection() const
{
    if (target.find(hardwareStr) == std::string::npos)
    {
        return Direction::NoDirection;
    }

    if (target.find(negativeStr) != std::string::npos)
    {
        return Direction::Negative;
    }

    if (target.find(positiveStr) != std::string::npos)
    {
        return Direction::Positive;
    }

    return Direction::NoDirection;
}

std::string Binding::getTargetDisplayName() const
{
    std::string result = target;

    StrUtil::replace(result, "-", " ");

    StrUtil::replace(result, hardwareStr, "");
    StrUtil::replace(result, negativeStr, " -");
    StrUtil::replace(result, positiveStr, " +");

    StrUtil::replace(result, "ampersand octothorpe", "&#");
    StrUtil::replace(result, "hyphen exclamation mark", "-!");
    StrUtil::replace(result, "parentheses", "()");
    StrUtil::replace(result, " plus ", " + ");

    if (result.size() > 15)
    {
        if (auto pos = result.find(" or"); pos != std::string::npos)
        {
            result.erase(pos, result.size());
        }
    }

    result = result.substr(0, 15);

    return result;
}

void mpc::input::midi::to_json(json &j, const Binding &b)
{
    j = json{{targetKey, b.getTarget()},
             {messageTypeKey, messageTypeToString(b.getMessageType())},
             {midiNumberKey, b.getMidiNumber()},
             {midiChannelIndexKey, b.getMidiChannelIndex()}};

    if (b.getTarget() == "hardware:data-wheel")
    {
        j[encoderModeKey] = encoderModeToString(b.getEncoderMode());
        return;
    }

    if (b.getMessageType() == BindingMessageType::Controller)
    {
        if (b.isButtonLike())
        {
            j[midiValueKey] = b.getMidiValue();
        }
    }
}

void mpc::input::midi::from_json(const json &j, Binding &b)
{
    std::string target, type;
    j.at(targetKey).get_to(target);
    j.at(messageTypeKey).get_to(type);

    b.setTarget(target);
    b.setMessageType(stringToMessageType(type));
    b.setMidiNumber(j.at(midiNumberKey).get<int>());
    b.setMidiChannelIndex(j.at(midiChannelIndexKey).get<int>());

    const bool isController = b.isController();
    const bool hasMidiValue = j.contains(midiValueKey);
    bool shouldContainMidiValue = false;

    if (target == "hardware:data-wheel")
    {
        if (!j.contains(encoderModeKey))
        {
            throw std::invalid_argument(
                "Data wheel binding requires 'encoderMode'");
        }

        b.setEncoderMode(
            stringToEncoderMode(j.at(encoderModeKey).get<std::string>()));

        if (hasMidiValue)
        {
            throw std::invalid_argument(
                "Data wheel binding must not include 'midiValue'");
        }
    }
    else if (const auto hardwareTargetStr = b.getHardwareTarget())
    {
        const auto id = hardware::componentLabelToId.at(*hardwareTargetStr);

        if (j.contains(encoderModeKey))
        {
            throw std::invalid_argument(
                "encoderMode is only allowed for hardware:data-wheel");
        }

        const bool isButtonHardware = hardware::isButtonId(id);
        const bool isDataWheelDir = target == "hardware:data-wheel:negative" ||
                                    target == "hardware:data-wheel:positive";

        const bool isButtonLike = b.isButtonLike();

        if (isController && isButtonLike)
        {
            if (!hasMidiValue)
            {
                throw std::invalid_argument(
                    "Button-like binding requires 'midiValue'");
            }

            const auto candidate = j.at(midiValueKey).get<int>();

            if (candidate < 1 || candidate > 127)
            {
                throw std::invalid_argument(
                    "'midiValue' must be between 1 and 127, but it is " +
                    std::to_string(candidate));
            }

            b.setMidiValue(candidate);
            shouldContainMidiValue = true;
        }
        else
        {
            if (hasMidiValue)
            {
                throw std::invalid_argument(
                    "Binding must not include 'midiValue'");
            }
        }
    }
    else
    {
        if (j.contains(encoderModeKey))
        {
            throw std::invalid_argument(
                "encoderMode is only allowed for hardware:data-wheel");
        }
    }

    if (!shouldContainMidiValue && hasMidiValue)
    {
        throw std::invalid_argument("Binding must not include 'midiValue'");
    }
}

void MidiControlPresetV3::setVersion(long long v)
{
    version = ConstrainedInt<long long, 0, 4503599627370496LL>{v};
}

void MidiControlPresetV3::setName(const std::string &n)
{
    static const std::regex pattern("^[A-Za-z0-9 !#\\$%&'\\(\\)\\-@_{}]+$");
    if (n.empty() || n.size() > 16 || !std::regex_match(n, pattern))
    {
        throw std::invalid_argument("Invalid name (must match schema)");
    }
    name = n;
}

void MidiControlPresetV3::setMidiControllerDeviceName(const std::string &n)
{
    static const std::regex pattern("^[a-zA-Z0-9 _\\-\\.\\(\\)\\/\\,;]*$");
    if (n.size() > 255 || !std::regex_match(n, pattern))
    {
        throw std::invalid_argument("Invalid midiControllerDeviceName");
    }
    midiControllerDeviceName = n;
}

void MidiControlPresetV3::setAutoLoadMode(const AutoLoadMode m)
{
    if (m != AutoLoadModeNo && m != AutoLoadModeAsk && m != AutoLoadModeYes)
    {
        throw std::invalid_argument("Invalid AutoLoadMode");
    }

    autoLoadMode = m;
}

void MidiControlPresetV3::setBindings(const std::vector<Binding> &b)
{
    auto availableTargets = MidiControlPresetUtil::load_available_targets();
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

long long MidiControlPresetV3::getVersion() const
{
    return version;
}

const std::string &MidiControlPresetV3::getName() const
{
    return name;
}

const std::string &MidiControlPresetV3::getMidiControllerDeviceName() const
{
    return midiControllerDeviceName;
}

AutoLoadMode MidiControlPresetV3::getAutoLoadMode() const
{
    return autoLoadMode;
}

const std::vector<Binding> &MidiControlPresetV3::getBindings() const
{
    return bindings;
}

Binding &MidiControlPresetV3::getBindingByIndex(const int idx)
{
    return bindings[idx];
}

bool MidiControlPresetV3::hasBindingForNote(const int noteNumber) const
{
    for (auto &b : bindings)
    {
        if (b.isNote() && b.getMidiNumber() == noteNumber)
        {
            return true;
        }
    }

    return false;
}

void mpc::input::midi::to_json(json &j, const MidiControlPresetV3 &p)
{
    j = json{{versionKey, p.getVersion()},
             {nameKey, p.getName()},
             {midiControllerDeviceNameKey, p.getMidiControllerDeviceName()},
             {autoLoadModeKey, autoLoadModeToString(p.getAutoLoadMode())},
             {bindingsKey, p.getBindings()}};
}

void mpc::input::midi::from_json(const json &j, MidiControlPresetV3 &p)
{
    static const std::set<std::string> allowedKeys = {
        versionKey, nameKey, midiControllerDeviceNameKey, autoLoadModeKey,
        bindingsKey};

    for (auto &[key, _] : j.items())
    {
        if (allowedKeys.count(key) == 0)
        {
            throw std::invalid_argument(
                "Unknown key in MidiControlPresetV3 JSON: " + key);
        }
    }

    auto v = j.at(versionKey).get<long long>();
    if (v > CURRENT_PRESET_VERSION)
    {
        throw std::runtime_error(
            "Preset version is newer than this application supports.");
    }

    p.setVersion(v);
    p.setName(j.at(nameKey).get<std::string>());

    if (j.contains(midiControllerDeviceNameKey))
    {
        p.setMidiControllerDeviceName(
            j.at(midiControllerDeviceNameKey).get<std::string>());
    }

    p.setAutoLoadMode(
        stringToAutoLoadMode(j.at(autoLoadModeKey).get<std::string>()));
    p.setBindings(j.at(bindingsKey).get<std::vector<Binding>>());
}
