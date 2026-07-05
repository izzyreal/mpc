#include "LoadASetScreen.hpp"

#include "Mpc.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include <exception>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASetScreen::LoadASetScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-set", layerIndex)
{
}

LoadASetScreen::~LoadASetScreen() = default;

void LoadASetScreen::open()
{
    preview.reset();
    conversionTable = file::kaitai::Mpc60SetProgramLoader::defaultConversionTable();

    try
    {
        preview = std::make_unique<file::kaitai::Mpc60SetPreview>(
            file::kaitai::Mpc60SetPreviewLoader::loadPreview(
                mpc.screens->get<ScreenId::LoadScreen>()->getSelectedFile()));
    }
    catch (const std::exception &)
    {
        const auto layeredScreen = ls.lock();
        layeredScreen->postToUiThread(utils::Task(
            [layeredScreen]
            {
                layeredScreen->showPopupAndThenOpen(ScreenId::LoadScreen,
                                                    "Can't read SET", 1000);
            }));
    }
}

void LoadASetScreen::function(const int i)
{
    switch (i)
    {
        case 2:
            openScreenById(ScreenId::LoadASetSoundScreen);
            break;
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
            openScreenById(ScreenId::ConversionTableScreen);
            break;
        default:;
    }
}

const mpc::file::kaitai::Mpc60SetPreview *LoadASetScreen::getPreview() const
{
    return preview.get();
}

const mpc::file::kaitai::Mpc60SetProgramLoader::ConversionTable &
LoadASetScreen::getConversionTable() const
{
    return conversionTable;
}

mpc::DrumNoteNumber LoadASetScreen::getConversionTargetNote(
    const size_t mpc60PadIndex) const
{
    return conversionTable.at(mpc60PadIndex);
}

void LoadASetScreen::setConversionTargetNote(const size_t mpc60PadIndex,
                                             const DrumNoteNumber note)
{
    conversionTable.at(mpc60PadIndex) = note;
}
