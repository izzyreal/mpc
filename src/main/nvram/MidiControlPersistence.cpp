#include "MidiControlPersistence.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "Paths.hpp"
#include "disk/AbstractDisk.hpp"
#include "file/File.hpp"
#include "file/Directory.hpp"
#include "lang/StrUtil.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;
using namespace moduru::file;
using namespace moduru::lang;

std::vector<std::shared_ptr<MidiControlPreset>> MidiControlPersistence::presets;

void MidiControlPersistence::restoreLastState(mpc::Mpc& mpc)
{
    loadDefaultMapping(mpc);

    File f(mpc::Paths::configPath() + "midicontrolmapping.vmp", {});

    if (f.exists())
    {
        try
        {
            auto &preset = mpc.screens->get<VmpcMidiScreen>("vmpc-midi")->activePreset;
            mpc.getDisk()->readMidiControlPreset(f, preset);
            f.close();
        }
        catch (const std::exception& e)
        {
            MLOG("Error while loading NVRAM MIDI control preset: " + std::string(e.what()));
            MLOG("The default preset will be loaded instead.");
            loadDefaultMapping(mpc);
        }
    }

    f.close();

    {
        // Temporary hack introduced in v0.5.0.1 (v0.5.0-RC1) to address the fact
        // that people have persisted the insane default preset that was introduced
        // by v0.5.0.0 (v0.5.0-RC0). We check if at least 4 of the pad mappings are
        // bad, and if yes, we load the new and sane default mapping.
        int badMappingCounter = 0;

        auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
        auto &rows = vmpcMidiScreen->activePreset->rows;

        for (auto &r: rows)
        {
            if (r.label.length() < 4 || r.label.substr(0, 4) != "pad-") continue;

            const bool isBadOldDefault = r.isNote == false || r.value == -1;
            if (isBadOldDefault) badMappingCounter += 1;
        }

        if (badMappingCounter >= 4) loadDefaultMapping(mpc);
    }
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
    labels.emplace_back("datawheel-up");
    labels.emplace_back("datawheel-down");
    labels.emplace_back("slider");
    labels.emplace_back("rec-gain");
    labels.emplace_back("main-volume");

    for (auto& b : hardware->getButtonLabels())
    {
        labels.emplace_back(b);
    }

    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
    vmpcMidiScreen->activePreset->rows.clear();

    for (auto& label : labels)
    {
        MidiControlCommand command {label, false, -1, -1 };

        if (label.length() >= 4 && label.substr(0, 4) == "pad-")
        {
            char pad = 1;

            try {
                const int numberStringWidth = label.length() == 5 ? 1 : 2;
                pad = stoi(label.substr(4, numberStringWidth));
            } catch (const std::exception&) {}

            command.isNote = true;
            command.value = 34 + pad;
        }
        else if (label == "slider")
        {
            command.value = 7;
        }

        vmpcMidiScreen->updateOrAddActivePresetCommand(command);
    }
}

void MidiControlPersistence::saveCurrentState(mpc::Mpc& mpc)
{
    const auto path = mpc::Paths::configPath() + "midicontrolmapping.vmp";

    try
    {
        File f(path, {});
        saveVmpcMidiScreenPresetToFile(mpc, f, "currentstate");
    }
    catch (const std::exception& e)
    {
        MLOG("Error while saving NVRAM MIDI control preset: " + std::string(e.what()));
        MLOG("If the error persists, delete " + path + ", if it exists." );
    }
}

void MidiControlPersistence::saveVmpcMidiScreenPresetToFile(mpc::Mpc &mpc, File &f, std::string name)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    std::vector<char> data;

    auto preset = vmpcMidiScreen->activePreset;

    data.push_back(preset->autoloadMode);

    for (int i = 0; i < name.length(); i++)
    {
        data.push_back(name[i]);
    }

    for (int i = name.length(); i < 16; i++)
    {
        data.push_back(' ');
    }

    for (auto& c : preset->rows)
    {
        MidiControlCommand r { c.label, c.isNote, c.channel, c.value };

        auto rowData = r.toBytes();

        for (auto& b : rowData)
            data.push_back(b);
    }

    f.setData(&data);
    f.close();
}

void MidiControlPersistence::loadFileByNameIntoPreset(
        mpc::Mpc& mpc,
        std::string name,
        std::shared_ptr<MidiControlPreset> preset
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
            mpc.getDisk()->readMidiControlPreset(*f.get(), preset);
            break;
        }
    }
}

void MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(mpc::Mpc& mpc)
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
        auto preset = presets.emplace_back(std::make_shared<MidiControlPreset>());
        mpc.getDisk()->readMidiControlPreset(*f.get(), preset);
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

bool MidiControlPersistence::doesPresetWithNameExist(std::string name)
{
    auto dir = std::make_shared<Directory>(mpc::Paths::midiControlPresetsPath(), nullptr);

    for (auto& node : dir->listFiles())
    {
        if (node->isDirectory())
        {
            continue;
        }

        auto f = std::dynamic_pointer_cast<File>(node);

        if (StrUtil::eqIgnoreCase(f->getNameWithoutExtension(), name))
        {
            return true;
        }
    }

    return false;
}
