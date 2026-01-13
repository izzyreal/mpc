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

std::vector<std::string> MidiControlPresetUtil::load_available_targets()
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
    std::vector<std::string> targets;
    for (const auto &v : enumArray)
    {
        targets.push_back(v.get<std::string>());
    }

    return targets;
}

void MidiControlPresetUtil::resetMidiControlPreset(
    std::shared_ptr<mpc::input::midi::MidiControlPresetV3> p)
{
    const json schema = load_schema();
    const auto targets = load_available_targets();

    p->setVersion(CURRENT_PRESET_VERSION);

    const json &rootProps = schema["properties"];
    const json &bindingSchema = schema["$defs"]["binding"];

    if (rootProps.contains("autoLoad") &&
        rootProps["autoLoad"].contains("default"))
    {
        p->setAutoLoad(rootProps["autoLoad"]["default"].get<std::string>());
    }

    std::vector<Binding> bindings;
    bindings.reserve(targets.size());

    for (const auto &target : targets)
    {
        Binding b;
        b.setTarget(target);

        if (bindingSchema.contains("default"))
        {
            const json &d = bindingSchema["default"];

            if (d.contains("messageType"))
            {
                b.setMessageType(
                    stringToMessageType(d["messageType"].get<std::string>()));
            }

            if (d.contains("midiNumber"))
            {
                b.setMidiNumber(d["midiNumber"].get<int>());
            }

            if (d.contains("midiValue"))
            {
                b.setMidiValue(d["midiValue"].get<int>());
            }

            if (d.contains("midiChannelIndex"))
            {
                b.setMidiChannelIndex(d["midiChannelIndex"].get<int>());
            }

            if (d.contains("enabled"))
            {
                b.setEnabled(d["enabled"].get<bool>());
            }
        }

        bindings.push_back(b);
    }

    p->setBindings(bindings);
}

bool MidiControlPresetUtil::doesPresetWithNameExist(const fs::path &path,
                                                    std::string name)
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

void MidiControlPresetUtil::ensurePresetHasAllAvailableTargets(
    std::shared_ptr<mpc::input::midi::MidiControlPresetV3> p)
{
    const auto targets = load_available_targets();
    const json schema = load_schema();
    const json &bindingSchema = schema["$defs"]["binding"];

    auto bindings = p->getBindings();

    for (const auto &target : targets)
    {
        auto it = std::find_if(bindings.begin(), bindings.end(),
                               [&](const Binding &b)
                               {
                                   return b.getTarget() == target;
                               });

        if (it != bindings.end())
        {
            continue;
        }

        Binding b;
        b.setTarget(target);

        if (bindingSchema.contains("default"))
        {
            const json &d = bindingSchema["default"];

            if (d.contains("messageType"))
            {
                b.setMessageType(
                    stringToMessageType(d["messageType"].get<std::string>()));
            }

            if (d.contains("midiNumber"))
            {
                b.setMidiNumber(d["midiNumber"].get<int>());
            }

            if (d.contains("midiValue"))
            {
                b.setMidiValue(d["midiValue"].get<int>());
            }

            if (d.contains("midiChannelIndex"))
            {
                b.setMidiChannelIndex(d["midiChannelIndex"].get<int>());
            }

            if (d.contains("enabled"))
            {
                b.setEnabled(d["enabled"].get<bool>());
            }
        }

        bindings.push_back(b);
    }

    p->setBindings(bindings);
}

void MidiControlPresetUtil::ensureTargetsAreInSameOrderAsInSchema(
    std::shared_ptr<mpc::input::midi::MidiControlPresetV3> p)
{
    const auto targets = load_available_targets();
    auto bindings = p->getBindings();

    std::unordered_map<std::string, size_t> order;
    for (size_t i = 0; i < targets.size(); i++)
    {
        order[targets[i]] = i;
    }

    std::stable_sort(bindings.begin(), bindings.end(),
                     [&](const Binding &a, const Binding &b)
                     {
                         return order[a.getTarget()] < order[b.getTarget()];
                     });

    p->setBindings(bindings);
}
