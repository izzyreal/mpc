#include "input/midi/MidiControlPresetV3.hpp"

#include "IntTypes.hpp"

#include "StrUtil.hpp"

#include "input/midi/MidiControlPresetUtil.hpp"

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

void Binding::setMessageType(const std::string &t)
{
    if (t != "CC" && t != "Note")
    {
        throw std::invalid_argument("messageType must be 'CC' or 'Note'");
    }
    messageType = t;
}

void Binding::setMidiNumber(int n)
{
    if (n < 0)
    {
        midiNumber = MidiNumber(0);
        enabled = false;
    }
    else
    {
        midiNumber = MidiNumber(n);
        enabled = true;
    }
}

void Binding::setMidiValue(int v)
{
    midiValue = MidiValue(v);
}

void Binding::setMidiChannelIndex(int ch)
{
    midiChannelIndex = MidiChannel(ch);
}

void Binding::setEnabled(bool e)
{
    enabled = e;
}

const std::string &Binding::getTarget() const
{
    return target;
}

const std::string &Binding::getMessageType() const
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
    return enabled;
}

bool Binding::isCc() const
{
    return getMessageType() == "CC";
}

bool Binding::isNote() const
{
    return getMessageType() == "Note";
}

std::string Binding::getTargetDisplayName() const
{
    static const std::string hardwareStr = "hardware:";
    static const std::string sequencerStr = "sequencer:";
    static const std::string negativeStr = ":negative";
    static const std::string positiveStr = ":positive";

    std::string result = target;

    StrUtil::replace(result, "-", " ");

    StrUtil::replace(result, hardwareStr, "");
    StrUtil::replace(result, sequencerStr, "seq ");
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

    printf("target: %s, display name: %s\n", target.c_str(), result.c_str());

    return result;
}

void mpc::input::midi::to_json(json &j, const Binding &b)
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

void mpc::input::midi::from_json(const json &j, Binding &b)
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
