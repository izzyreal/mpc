#include "ConversionTableScreen.hpp"

#include "Mpc.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "StrUtil.hpp"
#include "lcdgui/screens/window/LoadASetScreen.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Sampler.hpp"

#include <algorithm>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

ConversionTableScreen::ConversionTableScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "conversion-table", layerIndex)
{
}

void ConversionTableScreen::open()
{
    displayPad();
    displayBecomesNote();
}

void ConversionTableScreen::turnWheel(const int i)
{
    const auto loadASetScreen = mpc.screens->get<ScreenId::LoadASetScreen>();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "mpc60-pad")
    {
        selectedMpc60PadIndex = static_cast<size_t>(std::clamp(
            static_cast<int>(selectedMpc60PadIndex) + i, 0,
            33));
        displayPad();
        displayBecomesNote();
    }
    else if (focusedFieldName == "becomes-note")
    {
        const auto note = loadASetScreen->getConversionTargetNote(selectedMpc60PadIndex);
        loadASetScreen->setConversionTargetNote(
            selectedMpc60PadIndex,
            DrumNoteNumber(std::clamp(static_cast<int>(note) + i,
                                      static_cast<int>(MinDrumNoteNumber),
                                      static_cast<int>(MaxDrumNoteNumber))));
        displayBecomesNote();
    }
}

void ConversionTableScreen::displayPad() const
{
    auto padLabel =
        file::kaitai::Mpc60SetPreview::mpc60PadName(selectedMpc60PadIndex);
    std::replace(padLabel.begin(), padLabel.end(), '_', ' ');
    padLabel = StrUtil::padRight(StrUtil::toUpper(padLabel), " ", 10) + "(" +
               file::kaitai::Mpc60SetPreview::mpc60SourceSlotLabel(
                   selectedMpc60PadIndex) +
               ")";
    findField("mpc60-pad")->setText(StrUtil::padRight(padLabel, " ", 18));
}

void ConversionTableScreen::displayBecomesNote() const
{
    const auto loadASetScreen = mpc.screens->get<ScreenId::LoadASetScreen>();
    const auto note = loadASetScreen->getConversionTargetNote(selectedMpc60PadIndex);
    const auto program = getProgram();
    const auto padIndices =
        program ? program->getPadIndicesFromNote(note)
                : std::vector<ProgramPadIndex>{};
    const auto padName = padIndices.empty()
                             ? std::string("OFF")
                             : sampler.lock()->getPadName(
                                   static_cast<int>(padIndices.front()));
    findField("becomes-note")->setText(std::to_string(note) + "/" + padName);
}

void ConversionTableScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
            openScreenById(ScreenId::LoadASetReplaceAddScreen);
            break;
        default:;
    }
}
