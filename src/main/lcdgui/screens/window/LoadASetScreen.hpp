#pragma once
#include "file/kaitai/Mpc60SetProgramLoader.hpp"
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::file::kaitai
{
    struct Mpc60SetPreview;
}

namespace mpc::lcdgui::screens::window
{
    class LoadASetScreen final : public ScreenComponent
    {
    public:
        LoadASetScreen(Mpc &mpc, int layerIndex);
        ~LoadASetScreen() override;

        void open() override;
        void function(int i) override;

        const file::kaitai::Mpc60SetPreview *getPreview() const;
        const file::kaitai::Mpc60SetProgramLoader::ConversionTable &
        getConversionTable() const;
        DrumNoteNumber getConversionTargetNote(size_t mpc60PadIndex) const;
        void setConversionTargetNote(size_t mpc60PadIndex, DrumNoteNumber note);

    private:
        std::unique_ptr<file::kaitai::Mpc60SetPreview> preview;
        file::kaitai::Mpc60SetProgramLoader::ConversionTable conversionTable =
            file::kaitai::Mpc60SetProgramLoader::defaultConversionTable();
    };
} // namespace mpc::lcdgui::screens::window
