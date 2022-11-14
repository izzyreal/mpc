#include "MidiControlPersistence.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "Paths.hpp"
#include "file/File.hpp"
#include "file/Directory.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;

void MidiControlPersistence::restoreLastState(mpc::Mpc& mpc)
{
    loadDefaultMapping(mpc);

    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});

    if (f.exists())
    {
        loadMappingFromFile(mpc, f);
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
    vmpcMidiScreen->labelCommands.clear();

    for (auto& label : labels)
    {
        VmpcMidiScreen::Command command { false, -1, -1 };
        vmpcMidiScreen->setLabelCommand(label, command);
    }
}

void MidiControlPersistence::saveCurrentState(mpc::Mpc& mpc)
{
    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});
    saveMappingToFile(mpc, f);
}

void MidiControlPersistence::loadMappingFromFile(mpc::Mpc &mpc, const std::string& name)
{
    auto dir = std::make_shared<Directory>(mpc::Paths::midiControlPresetsPath(), nullptr);

    for (auto& node : dir->listFiles())
    {
        if (node->isDirectory()) continue;
        auto f = std::dynamic_pointer_cast<File>(node);
        if (f->getNameWithoutExtension() == name)
        {
            loadMappingFromFile(mpc, *f);
        }
    }
}

void MidiControlPersistence::saveMappingToFile(mpc::Mpc &mpc, const std::string& name)
{
    Directory dir(mpc::Paths::midiControlPresetsPath(), {});

    if (!dir.exists())
    {
        dir.create();
    }

    File f(mpc::Paths::midiControlPresetsPath() + name, {});

    if (f.exists())
    {
        f.del();
        f.create();
    }

    saveMappingToFile(mpc, f);
}

void MidiControlPersistence::loadMappingFromFile(mpc::Mpc &mpc, File& f)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    std::vector<std::pair<std::string, VmpcMidiScreen::Command>> commands;

    loadMappingFromFile(f, commands);

    for (auto& command : commands)
        vmpcMidiScreen->setLabelCommand(command.first, command.second);
}

void MidiControlPersistence::saveMappingToFile(mpc::Mpc &mpc, File &f)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    auto commands = vmpcMidiScreen->labelCommands;

    std::vector<char> data;

    for (auto& c : commands)
    {
        auto label = c.first;
        auto isNote = c.second.isNote;
        auto channel = c.second.channelIndex;
        auto value = c.second.value;
        Row r {label, isNote, static_cast<char>(channel), static_cast<char>(value) };
        auto rowData = r.toBytes();

        for (auto& b : rowData)
            data.push_back(b);
    }

    f.setData(&data);
    f.close();
}

std::vector<std::string> MidiControlPersistence::getAvailablePresetNames()
{
    std::vector<std::string> result;
    auto dir = std::make_shared<Directory>(mpc::Paths::midiControlPresetsPath(), nullptr);

    if (!dir->exists())
    {
        return {};
    }

    for (auto& node : dir->listFiles())
    {
        if (node->isDirectory()) continue;
        result.emplace_back(node->getNameWithoutExtension());
    }

    return result;
}

void MidiControlPersistence::loadMappingFromFile(File& f,
                                                 std::vector<std::pair<std::string, VmpcMidiScreen::Command>>& commands)
{
    auto data = std::vector<char>(f.getLength());
    f.getData(&data);
    int pointer = 0;

    while (true)
    {
        if (pointer >= data.size())
        {
            break;
        }

        std::string name;
        char c;

        while ((c = data[pointer++]) != ' ' && pointer < data.size())
        {
            name.push_back(c);
        }

        const bool isNote = data[pointer++] == 1;
        const char channel = data[pointer++];
        const char value = data[pointer++];

        commands.emplace_back(std::pair<std::string, VmpcMidiScreen::Command>(name, {isNote, channel, value}));
    }
}
