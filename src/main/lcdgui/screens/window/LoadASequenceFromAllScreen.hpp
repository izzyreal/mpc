#pragma once

#include "lcdgui/ScreenComponent.hpp"
#include "sequencer/SequenceMetaInfo.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadASequenceFromAllScreen final : public ScreenComponent
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;

        LoadASequenceFromAllScreen(Mpc &mpc, int layerIndex);

        void open() override;

        std::vector<sequencer::SequenceMetaInfo> sequenceMetaInfos;

    private:
        int sourceSeqIndex = 0;
        void setSourceSeqIndex(int i);
        void displayFile() const;
        void displayLoadInto() const;
    };
} // namespace mpc::lcdgui::screens::window
