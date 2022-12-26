#include "MidiControlPersistence.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "Paths.hpp"
#include "file/File.hpp"
#include "file/Directory.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;

std::vector<MidiControlPreset> MidiControlPersistence::presets;

void MidiControlPersistence::restoreLastState(mpc::Mpc& mpc)
{
    loadDefaultMapping(mpc);

    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});

    if (f.exists())
    {
        auto& preset = mpc.screens->get<VmpcMidiScreen>("vmpc-midi")->activePreset;
        loadFileIntoPreset(f, preset);
        f.close();
    }

    f.close();
}

void MidiControlPersistence::loadDefaultMapping(mpc::Mpc &mpc)
{
    std::vector<std::string> labels;

    auto hardware = mpc.getHardware();

    for (auto& p : hardware->getPads())
    {
        labels.push_back(p->getLabel());
    }

    labels.emplace_back("datawheel");
    labels.emplace_back("slider");
    labels.emplace_back("rec-gain");
    labels.emplace_back("main-volume");

    for (auto& b : hardware->getButtonLabels())
    {
        labels.emplace_back(b);
    }

    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
    vmpcMidiScreen->activePreset.rows.clear();

    for (auto& label : labels)
    {
        MidiControlCommand command {label, false, -1, -1 };
        vmpcMidiScreen->updateOrAddActivePresetCommand(command);
    }
}

void MidiControlPersistence::saveCurrentState(mpc::Mpc& mpc)
{
    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});
    saveVmpcMidiScreenPresetToFile(mpc, f, "currenstate", 1);
}

void MidiControlPersistence::saveCurrentMappingToFile(mpc::Mpc &mpc, const std::string& name, int autoloadMode)
{
    Directory dir(mpc::Paths::midiControlPresetsPath(), {});

    if (!dir.exists())
    {
        dir.create();
    }

    File f(mpc::Paths::midiControlPresetsPath() + name + ".vmp", {});

    if (f.exists())
    {
        f.del();
        f.create();
    }

    saveVmpcMidiScreenPresetToFile(mpc, f, name, autoloadMode);
}

void MidiControlPersistence::saveVmpcMidiScreenPresetToFile(mpc::Mpc &mpc, File &f, std::string name, int autoLoadMode)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    std::vector<char> data;

    data.push_back(autoLoadMode);

    for (int i = 0; i < name.length(); i++)
    {
        data.push_back(name[i]);
    }

    for (int i = name.length(); i < 16; i++)
    {
        data.push_back(' ');
    }

    for (auto& c : vmpcMidiScreen->activePreset.rows)
    {
        MidiControlCommand r { c.label, c.isNote, c.channel, c.value };

        auto rowData = r.toBytes();

        for (auto& b : rowData)
            data.push_back(b);
    }

    f.setData(&data);
    f.close();
}

void MidiControlPersistence::savePresetToFile(MidiControlPreset &preset)
{
    std::vector<char> data;

    data.push_back(preset.autoloadMode);

    for (int i = 0; i < preset.name.length(); i++)
    {
        data.push_back(preset.name[i]);
    }

    for (int i = preset.name.length(); i < 16; i++)
    {
        data.push_back(' ');
    }

    for (auto& c : preset.rows)
    {
        MidiControlCommand r { c.label, c.isNote, c.channel, c.value };

        auto rowData = r.toBytes();

        for (auto& b : rowData)
            data.push_back(b);
    }

    File f(mpc::Paths::midiControlPresetsPath() + preset.name + ".vmp", {});
    f.del();
    f.create();
    f.setData(&data);
    f.close();
}

void MidiControlPersistence::loadFileByNameIntoPreset(
        std::string name,
        MidiControlPreset& preset
)
{
    auto dir = std::make_shared<Directory>(mpc::Paths::midiControlPresetsPath(), nullptr);

    for (auto& node : dir->listFiles())
    {
        if (node->isDirectory())
        {
            continue;
        }

        auto f = std::dynamic_pointer_cast<File>(node);

        if (f->getNameWithoutExtension() == name)
        {
            loadFileIntoPreset(*f.get(), preset);
            break;
        }
    }
}

void MidiControlPersistence::loadFileIntoPreset(
        File& f,
        MidiControlPreset& preset
)
{
    preset.rows.clear();
    preset.name = "";

    auto data = std::vector<char>(f.getLength());

    f.getData(&data);

    preset.autoloadMode = data[0];

    std::string presetName;

    for (int i = 0; i < 16; i++)
    {
        if (data[i + 1] == ' ') continue;
        presetName.push_back(data[i + 1]);
    }

    preset.name = presetName;

    int pointer = 17;

    while (pointer < data.size())
    {
        std::string name;
        char c;

        while ((c = data[pointer++]) != ' ' && pointer < data.size())
        {
            name.push_back(c);
        }

        const bool isNote = data[pointer++] == 1;
        const char channel = data[pointer++];
        const char value = data[pointer++];

        auto cmd = MidiControlCommand{name, isNote, channel, value};
        preset.rows.emplace_back(cmd);
    }
}

void MidiControlPersistence::loadAllPresetsFromDiskIntoMemory()
{
    presets.clear();

    auto dir = std::make_shared<Directory>(mpc::Paths::midiControlPresetsPath(), nullptr);

    for (auto& node : dir->listFiles())
    {
        if (node->isDirectory())
        {
            continue;
        }

        auto f = std::dynamic_pointer_cast<File>(node);
        MidiControlPreset& preset = presets.emplace_back(MidiControlPreset());
        loadFileIntoPreset(*f.get(), preset);
    }
}

void MidiControlPersistence::deleteLastState()
{
    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});

    if (f.exists())
    {
        f.del();
    }
}
