#include "MidiMappingPersistence.hpp"

#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include "hardware/Hardware.hpp"
#include "Paths.hpp"
#include "file/File.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;

const auto midiMappingPersistencePath = mpc::Paths::configPath() + "midimapping.ini";

void MidiMappingPersistence::load(mpc::Mpc& mpc)
{
    moduru::file::File f(midiMappingPersistencePath, {});
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    if (f.exists())
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
            VmpcMidiScreen::Command command { isNote, channel, value };
            vmpcMidiScreen->setLabelCommand(name, command);
        }
    }
    else
    {
        auto hardware = mpc.getHardware().lock();
        auto labels = hardware->getButtonLabels();

        labels.push_back("datawheel");
        labels.push_back("slider");

        for (auto& label : labels)
        {
            VmpcMidiScreen::Command command { false, -1, -1 };
            vmpcMidiScreen->setLabelCommand(label, command);
        }
    }

    f.close();
}

void MidiMappingPersistence::save(mpc::Mpc& mpc)
{
    moduru::file::File f(midiMappingPersistencePath, {});
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