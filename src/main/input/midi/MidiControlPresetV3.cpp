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
    j = json{{"target", b.getTarget()},
             {"messageType", messageTypeToString(b.getMessageType())},
             {"midiNumber", b.getMidiNumber()},
             {"midiChannelIndex", b.getMidiChannelIndex()}};

    if (b.getTarget() == "hardware:data-wheel")
    {
        j["encoderMode"] = encoderModeToString(b.getEncoderMode());
        return;
    }

    if (b.getMessageType() == BindingMessageType::Controller)
    {
        if (b.isButtonLike())
        {
            j["midiValue"] = b.getMidiValue();
        }
    }
}

void mpc::input::midi::from_json(const json &j, Binding &b)
{
    std::string target, type;
    j.at("target").get_to(target);
    j.at("messageType").get_to(type);

    b.setTarget(target);
    b.setMessageType(stringToMessageType(type));
    b.setMidiNumber(j.at("midiNumber").get<int>());
    b.setMidiChannelIndex(j.at("midiChannelIndex").get<int>());

    const bool isController = b.isController();
    const bool hasMidiValue = j.contains("midiValue");
    bool shouldContainMidiValue = false;

    if (target == "hardware:data-wheel")
    {
        if (!j.contains("encoderMode"))
        {
            throw std::invalid_argument(
                "Data wheel binding requires 'encoderMode'");
        }

        b.setEncoderMode(
            stringToEncoderMode(j.at("encoderMode").get<std::string>()));

        if (hasMidiValue)
        {
            throw std::invalid_argument(
                "Data wheel binding must not include 'midiValue'");
        }
    }
    else if (const auto hardwareTargetStr = b.getHardwareTarget())
    {
        const auto id = hardware::componentLabelToId.at(*hardwareTargetStr);

        if (j.contains("encoderMode"))
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

            const auto candidate = j.at("midiValue").get<int>();

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
        if (j.contains("encoderMode"))
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

void MidiControlPresetV3::setAutoLoad(const std::string &a)
{
    if (a != "No" && a != "Ask" && a != "Yes")
    {
        throw std::invalid_argument("autoLoad must be one of: No, Ask, Yes");
    }
    autoLoad = a;
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

const std::string &MidiControlPresetV3::getAutoLoad() const
{
    return autoLoad;
}

const std::vector<Binding> &MidiControlPresetV3::getBindings() const
{
    return bindings;
}

Binding &MidiControlPresetV3::getBindingByIndex(const int idx)
{
    return bindings[idx];
}

void mpc::input::midi::to_json(json &j, const MidiControlPresetV3 &p)
{
    j = json{{"version", p.getVersion()},
             {"name", p.getName()},
             {"midiControllerDeviceName", p.getMidiControllerDeviceName()},
             {"autoLoad", p.getAutoLoad()},
             {"bindings", p.getBindings()}};
}

void mpc::input::midi::from_json(const json &j, MidiControlPresetV3 &p)
{
    static const std::set<std::string> allowedKeys = {
        "version", "name", "midiControllerDeviceName", "autoLoad", "bindings"};

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
