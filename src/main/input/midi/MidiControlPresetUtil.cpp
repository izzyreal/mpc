#include "input/midi/MidiControlPresetUtil.hpp"

#include "input/midi/MidiControlPresetV3.hpp"

#include "MpcResourceUtil.hpp"
#include "StrUtil.hpp"

using namespace mpc::input::midi;

json MidiControlPresetUtil::load_schema()
{
    auto schemaData = mpc::MpcResourceUtil::get_resource_data(
        "midicontrolpresets/vmpc2000xl_midi_control_preset.schema.v3.json");
    return json::parse(schemaData);
}

json_validator MidiControlPresetUtil::make_validator()
{
    const json schemaJson = load_schema();
    json_validator validator;
    validator.set_root_schema(schemaJson);
    return validator;
}

std::set<std::string> MidiControlPresetUtil::load_available_targets()
{
    auto schema = load_schema();
    if (!schema.contains("$defs") || !schema["$defs"].contains("binding") ||
        !schema["$defs"]["binding"].contains("properties") ||
        !schema["$defs"]["binding"]["properties"].contains("target") ||
        !schema["$defs"]["binding"]["properties"]["target"].contains("enum"))
    {
        throw std::runtime_error("Schema missing target enum at expected path");
    }

    const auto &enumArray =
        schema["$defs"]["binding"]["properties"]["target"]["enum"];
    std::set<std::string> targets;
    for (const auto &v : enumArray)
    {
        targets.insert(v.get<std::string>());
    }

    return targets;
}

void MidiControlPresetUtil::resetMidiControlPreset(
    std::shared_ptr<mpc::input::midi::MidiControlPresetV3> p)
{
    const json schema = load_schema();
    const auto targets = load_available_targets();

    if (!schema.contains("properties"))
    {
        throw std::runtime_error("Schema missing root properties");
    }

    if (!schema.contains("$defs") || !schema["$defs"].contains("binding"))
    {
        throw std::runtime_error("Schema missing binding definition");
    }

    p->setVersion(CURRENT_PRESET_VERSION);

    const json &rootProps = schema["properties"];

    const json &bindingProps =
        schema["$defs"]["binding"]["properties"];

    if (rootProps.contains("name") &&
        rootProps["name"].contains("default"))
    {
        p->setName(rootProps["name"]["default"].get<std::string>());
    }

    if (rootProps.contains("midiControllerDeviceName") &&
        rootProps["midiControllerDeviceName"].contains("default"))
    {
        p->setMidiControllerDeviceName(
            rootProps["midiControllerDeviceName"]["default"]
                .get<std::string>());
    }

    if (rootProps.contains("autoLoad") &&
        rootProps["autoLoad"].contains("default"))
    {
        p->setAutoLoad(
            rootProps["autoLoad"]["default"].get<std::string>());
    }

    std::vector<Binding> bindings;
    bindings.reserve(targets.size());

    for (const auto &target : targets)
    {
        Binding b;
        b.setTarget(target);

        if (bindingProps.contains("messageType") &&
            bindingProps["messageType"].contains("default"))
        {
            b.setMessageType(
                bindingProps["messageType"]["default"]
                    .get<std::string>());
        }

        if (bindingProps.contains("midiNumber") &&
            bindingProps["midiNumber"].contains("default"))
        {
            b.setMidiNumber(
                bindingProps["midiNumber"]["default"].get<int>());
        }

        if (bindingProps.contains("midiValue") &&
            bindingProps["midiValue"].contains("default"))
        {
            b.setMidiValue(
                bindingProps["midiValue"]["default"].get<int>());
        }

        if (bindingProps.contains("midiChannelIndex") &&
            bindingProps["midiChannelIndex"].contains("default"))
        {
            b.setMidiChannelIndex(
                bindingProps["midiChannelIndex"]["default"].get<int>());
        }

        if (bindingProps.contains("enabled") &&
            bindingProps["enabled"].contains("default"))
        {
            b.setEnabled(
                bindingProps["enabled"]["default"].get<bool>());
        }

        bindings.push_back(b);
    }

    p->setBindings(bindings);
}


bool MidiControlPresetUtil::doesPresetWithNameExist(const fs::path &path, std::string name)
{
    auto path_it = fs::directory_iterator(path);

    return std::any_of(fs::begin(path_it), fs::end(path_it),
                       [name](const fs::directory_entry &e)
                       {
                           return !fs::is_directory(e) &&
                                  mpc::StrUtil::eqIgnoreCase(
                                      e.path().stem().string(), name);
                       });
}
