#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadASequenceScreen final : public ScreenComponent
    {
    public:
        LoadASequenceScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int increment) override;
        void function(int i) override;
        void releasePlayPreview();
        void startTempPreview();

        SequenceIndex loadInto{MinSequenceIndex};
        void setLoadInto(SequenceIndex);

    private:
        SequenceIndex originalSelectedSequenceIndex{MinSequenceIndex};
        bool previewingTempSequence = false;

        void stopTempPreviewAndRestoreSelection();
        void displayLoadInto() const;
        void displayFile() const;
    };
} // namespace mpc::lcdgui::screens::window
