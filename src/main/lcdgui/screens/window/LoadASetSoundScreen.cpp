#include "LoadASetSoundScreen.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "file/kaitai/Mpc60SetSoundLoader.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASetScreen.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Sampler.hpp"
#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"

#include <algorithm>
#include <string>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASetSoundScreen::LoadASetSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-set-sound", layerIndex)
{
}

void LoadASetSoundScreen::open()
{
    assignedMpc60PadIndexes.clear();
    selectedAssignedPadIndex = 0;

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto file = loadScreen->getSelectedFile();
    if (file == nullptr)
    {
        ls.lock()->showPopupAndAwaitInteraction("No SET selected");
        return;
    }

    const auto setScreen = mpc.screens->get<ScreenId::LoadASetScreen>();
    const auto *preview = setScreen->getPreview();

    if (preview == nullptr)
    {
        ls.lock()->showPopupAndAwaitInteraction("Can't read SET");
        return;
    }

    assignedMpc60PadIndexes.reserve(
        preview->soundDirectoryEntryIndexByMpc60Pad.size());
    for (size_t padIndex = 0;
         padIndex < preview->soundDirectoryEntryIndexByMpc60Pad.size();
         ++padIndex)
    {
        assignedMpc60PadIndexes.push_back(padIndex);
    }

    displayMpc60Pad();
    displayFile();
}

void LoadASetSoundScreen::turnWheel(const int i)
{
    if (assignedMpc60PadIndexes.empty())
    {
        return;
    }

    const auto current = static_cast<int>(selectedAssignedPadIndex);
    selectedAssignedPadIndex = static_cast<size_t>(std::clamp(
        current + i, 0,
        static_cast<int>(assignedMpc60PadIndexes.size() - 1)));
    displayMpc60Pad();
    displayFile();
}

void LoadASetSoundScreen::displayMpc60Pad() const
{
    if (assignedMpc60PadIndexes.empty())
    {
        findField("mpc60-pad")->setText("");
        return;
    }

    const auto padIndex = assignedMpc60PadIndexes[selectedAssignedPadIndex];
    auto padName = file::kaitai::Mpc60SetPreview::mpc60PadName(padIndex);
    std::replace(padName.begin(), padName.end(), '_', ' ');
    padName = StrUtil::toUpper(padName);
    const auto sourceSlot =
        file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(padIndex);
    findField("mpc60-pad")->setText(
        StrUtil::padRight(padName, " ", 10) + "(" + sourceSlot + ")");
}

void LoadASetSoundScreen::displayFile() const
{
    const auto setScreen = mpc.screens->get<ScreenId::LoadASetScreen>();
    const auto *preview = setScreen->getPreview();
    const auto entryIndex = getSelectedSoundDirectoryEntryIndex();

    if (preview == nullptr || !entryIndex)
    {
        findLabel("file")->setText("File:(no assign)");
        return;
    }

    if (*entryIndex >= preview->soundDirectoryEntries.size() ||
        preview->soundDirectoryEntries[*entryIndex].lengthInSamples == 0)
    {
        findLabel("file")->setText("File:(no assign)");
        return;
    }

    findLabel("file")->setText(
        "File:" +
        StrUtil::padRight(preview->soundDirectoryEntries[*entryIndex].name, " ",
                          16));
}

std::optional<size_t>
LoadASetSoundScreen::getSelectedSoundDirectoryEntryIndex() const
{
    const auto setScreen = mpc.screens->get<ScreenId::LoadASetScreen>();
    const auto *preview = setScreen->getPreview();

    if (preview == nullptr || assignedMpc60PadIndexes.empty())
    {
        return std::nullopt;
    }

    const auto padIndex = assignedMpc60PadIndexes[selectedAssignedPadIndex];
    const auto entryIndex =
        preview->soundDirectoryEntryIndexByMpc60Pad[padIndex];

    if (entryIndex >= preview->soundDirectoryEntries.size())
    {
        return std::nullopt;
    }

    return entryIndex;
}

void LoadASetSoundScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            ls.lock()->closeCurrentScreen();
            break;
        case 4:
        {
            const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
            const auto file = loadScreen->getSelectedFile();

            if (file == nullptr)
            {
                ls.lock()->showPopupAndAwaitInteraction("No SET selected");
                break;
            }

            const auto entryIndex = getSelectedSoundDirectoryEntryIndex();
            if (!entryIndex)
            {
                break;
            }

            auto sound = sampler.lock()->addSound(44100);
            if (sound == nullptr)
            {
                break;
            }

            const auto soundOrError =
                file::kaitai::Mpc60SetSoundLoader::loadSoundDirectoryEntry(
                    file, *entryIndex, sound);

            if (!soundOrError.has_value())
            {
                sampler.lock()->deleteSound(sound);
                ls.lock()->showPopupAndAwaitInteraction("Can't read SET");
                break;
            }

            const auto msg = "LOADING " + sound->getName();
            ls.lock()->showPopupAndThenOpen(ScreenId::LoadASoundScreen, msg, 300);
            break;
        }
        default:;
    }
}
