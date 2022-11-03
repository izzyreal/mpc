#include "MidiMappingPersistence.hpp"

#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "Paths.hpp"
#include "file/File.hpp"
#include "file/Directory.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;

const auto midiMappingPersistencePath = mpc::Paths::configPath() + "midimapping.ini";
const auto midiMappingPresetsPath = mpc::Paths::midiControllerPresetsPath();

void MidiMappingPersistence::restoreLastState(mpc::Mpc& mpc)
{
    moduru::file::File f(midiMappingPersistencePath, {});

    if (f.exists())
    {
        loadMappingFromFile(mpc, f);
        f.close();
    }
    else
    {
        loadDefaultMapping(mpc);
    }

    f.close();
}

void MidiMappingPersistence::loadDefaultMapping(mpc::Mpc &mpc)
{
    std::vector<std::string> labels;

    auto hardware = mpc.getHardware().lock();

    for (auto& p : hardware->getPads())
    {
        labels.push_back(p->getLabel());
    }

    labels.push_back("datawheel");
    labels.push_back("slider");

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

void MidiMappingPersistence::saveCurrentState(mpc::Mpc& mpc)
{
    moduru::file::File f(midiMappingPersistencePath, {});
    saveMappingToFile(mpc, f);
}

void MidiMappingPersistence::loadMappingFromFile(mpc::Mpc &mpc, std::string name)
{
    moduru::file::Directory dir(midiMappingPresetsPath, nullptr);

    for (auto& node : dir.listFiles())
    {
        if (node->isDirectory()) continue;
        auto f = std::dynamic_pointer_cast<moduru::file::File>(node);
        if (f->getNameWithoutExtension() == name)
        {
            loadMappingFromFile(mpc, *f.get());
        }
    }
}

void MidiMappingPersistence::saveMappingToFile(mpc::Mpc &mpc, std::string name)
{
    auto dir = std::make_shared<moduru::file::Directory>(midiMappingPresetsPath, nullptr);
    moduru::file::File f(name, dir);

    if (f.exists())
    {
        f.del();
        f.create();
    }

    saveMappingToFile(mpc, f);
}

void MidiMappingPersistence::loadMappingFromFile(mpc::Mpc &mpc, moduru::file::File& f)
{
    auto data = std::vector<char>(f.getLength());
    f.getData(&data);
    int pointer = 0;

    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

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
        VmpcMidiScreen::Command command { isNote, channel, value };
        vmpcMidiScreen->setLabelCommand(name, command);
    }
}

void MidiMappingPersistence::saveMappingToFile(mpc::Mpc &mpc, moduru::file::File &f)
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

std::vector<std::string> MidiMappingPersistence::getAvailablePresetNames()
{
    std::vector<std::string> result;
    moduru::file::Directory dir(midiMappingPresetsPath, nullptr);

    for (auto& node : dir.listFiles())
    {
        if (node->isDirectory()) continue;
        result.emplace_back(node->getNameWithoutExtension());
    }

    return result;
}
